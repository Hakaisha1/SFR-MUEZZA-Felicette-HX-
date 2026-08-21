# 📖 Guide: Integration

Panduan teknis pengembangan untuk folder `/integration`. Baca `agent.md` terlebih dahulu sebelum dokumen ini.

> 📋 **Kontrak Data**: Semua format data antar divisi didefinisikan di [`data_contract.json`](../data_contract.json). Gunakan file tersebut sebagai referensi utama saat implementasi parser, sender, atau listener.
> 📐 **State Machine**: Lihat [`state_machine.md`](../state_machine.md) untuk diagram lengkap dua-layer (sub-state RPi5 + state STM32).
> 🔀 **Git Workflow**: Lihat [`flow.md`](../flow.md) untuk aturan branching dan PR.

---

## Setup Environment

### Dependensi Python (RPi5)
```bash
pip install pyserial
```

### Struktur Folder
```
/integration
├── agent.md              # Definisi peran
├── guide.md              # Dokumen ini
├── main.py               # Entry point, loop utama state machine
├── uart_driver.py        # Kirim/terima paket UART ke STM32
├── state_machine.py      # Logika perpindahan state robot
├── vision_listener.py    # Terima data dari proses Vision (socket/pipe)
└── tests/
    ├── test_uart.py      # Unit test kirim & terima paket
    └── test_state.py     # Unit test transisi state machine
```

---

## Cara Membangun Paket UART

Sesuai `protokol_komunikasi.json`, semua paket menggunakan format:

```
[0xAA] [0x55] [LENGTH] [MSG_ID] [PAYLOAD...] [CHECKSUM]
```

Checksum dihitung sebagai XOR dari `MSG_ID` dan semua byte `PAYLOAD`.

### Contoh Python: Membangun & Mengirim Paket
```python
import serial
import struct

ser = serial.Serial('/dev/ttyAMA0', baudrate=115200, timeout=0.1)

def hitung_checksum(msg_id: int, payload: bytes) -> int:
    checksum = msg_id
    for b in payload:
        checksum ^= b
    return checksum

def kirim_perintah(msg_id: int, payload: bytes):
    length = len(payload)
    checksum = hitung_checksum(msg_id, payload)
    paket = bytes([0xAA, 0x55, length, msg_id]) + payload + bytes([checksum])
    ser.write(paket)

# Contoh: kirim CMD_GERAK_OMNIDIRECTIONAL (0x10)
# maju 0.3 m/s, tidak crab walk, tidak rotasi
payload = struct.pack('<fff', 0.3, 0.0, 0.0)
kirim_perintah(0x10, payload)
```

### Contoh Python: Menerima Telemetri
```python
def baca_paket():
    """Membaca satu paket dari UART. Return (msg_id, payload) atau None."""
    buf = ser.read(2)
    if len(buf) < 2 or buf[0] != 0xAA or buf[1] != 0x55:
        return None
    length_byte = ser.read(1)
    if not length_byte:
        return None
    length = length_byte[0]
    sisa = ser.read(length + 2)  # MSG_ID + PAYLOAD + CHECKSUM
    if len(sisa) < length + 2:
        return None
    msg_id = sisa[0]
    payload = sisa[1:1 + length]
    checksum_diterima = sisa[-1]
    checksum_dihitung = hitung_checksum(msg_id, payload)
    if checksum_diterima != checksum_dihitung:
        return None  # Paket korup, abaikan
    return msg_id, payload
```

---

## Implementasi State Machine

State machine menggunakan **dua layer** (lihat [`state_machine.md`](../state_machine.md)):
- **Sub-state RPi5** — logika detail di `state_machine.py` (SEARCHING, APPROACHING, CLASSIFYING, GRIPPING, CARRYING, RELEASING, ERROR_RETRY)
- **State STM32** — yang dikirim via `CMD_STATE_CONTROL` (`0x12`): IDLE(0), WALKING(1), EVACUATING(2), EMERGENCY_STOP(3)

Gunakan pola berikut sebagai dasar `state_machine.py`:

```python
from enum import Enum

# Sub-state RPi5 (logika detail di sisi Integration)
class SubState(Enum):
    SEARCHING = 0
    APPROACHING = 1
    CLASSIFYING = 2
    GRIPPING = 3
    CARRYING = 4
    RELEASING = 5
    ERROR_RETRY = 6
    EMERGENCY_STOP = 7

# Mapping sub-state RPi5 → state STM32 (dikirim via CMD 0x12)
STM32_STATE_MAP = {
    SubState.SEARCHING:      1,  # WALKING
    SubState.APPROACHING:    1,  # WALKING
    SubState.CLASSIFYING:    0,  # IDLE
    SubState.GRIPPING:       2,  # EVACUATING
    SubState.CARRYING:       2,  # EVACUATING
    SubState.RELEASING:      2,  # EVACUATING
    SubState.ERROR_RETRY:    0,  # IDLE
    SubState.EMERGENCY_STOP: 3,  # EMERGENCY_STOP
}

class StateMachine:
    def __init__(self):
        self.state = SubState.SEARCHING

    def update(self, data_vision, data_telemetry):
        """Dipanggil setiap loop. Return command untuk dikirim ke STM32."""
        if data_telemetry.get('hardware_error'):
            self.state = SubState.EMERGENCY_STOP
            return self._command_state(3)

        if self.state == SubState.SEARCHING:
            if data_vision['label'] in ('dummy', 'riil') and data_vision['confidence'] > 0.75:
                self.state = SubState.APPROACHING
        elif self.state == SubState.APPROACHING:
            if data_vision['jarak_estimasi_cm'] < 15:
                self.state = SubState.CLASSIFYING
                return self._command_state(0)  # IDLE — robot berhenti untuk klasifikasi
        elif self.state == SubState.CLASSIFYING:
            if data_vision['label'] == 'dummy':
                self.state = SubState.SEARCHING
                return self._command_state(1)  # WALKING
            elif data_vision['label'] == 'riil':
                self.state = SubState.GRIPPING
                return self._command_state(2)  # EVACUATING
        # ... tambahkan transisi GRIPPING → CARRYING → RELEASING

    def _command_state(self, state_target: int):
        return {'msg_id': 0x12, 'state_target': state_target}
```

---

## Watchdog (Timeout Handling)

STM32 akan masuk `EMERGENCY_STOP` otomatis jika tidak menerima paket dalam **500ms**. Oleh karena itu:

```python
import time

INTERVAL_HEARTBEAT_S = 0.1  # Kirim command tiap 100ms (10 Hz)

while True:
    tik_mulai = time.monotonic()

    data_vision = vision_listener.ambil_data_terbaru()
    data_telemetry = uart_driver.baca_telemetri()
    command = state_machine.update(data_vision, data_telemetry)
    uart_driver.kirim(command)

    elapsed = time.monotonic() - tik_mulai
    time.sleep(max(0, INTERVAL_HEARTBEAT_S - elapsed))
```

---

## Checklist Integrasi Bertahap (Sesuai Milestone)

**M0 — Setup:**
- [ ] Skeleton UART dapat kirim dan terima data dummy tanpa error
- [ ] Checksum berfungsi — paket korup berhasil dideteksi dan diabaikan

**M1 — Fondasi:**
- [ ] State machine skeleton: transisi SEARCHING → APPROACHING berjalan dengan data placeholder
- [ ] Watchdog terbukti aktif: cabut kabel UART → robot berhenti dalam 500ms

**M3 — Integrasi Awal:**
- [ ] Vision output live tersambung ke state machine RPi5
- [ ] Command asli terkirim ke STM32, robot bergerak sesuai sub-state

**M4 — Full Pipeline:**
- [ ] Full pipeline: SEARCHING → APPROACHING → CLASSIFYING → GRIPPING → CARRYING → RELEASING berjalan end-to-end
- [ ] Telemetri `TLM_ROBOT_STATUS` & `TLM_SENSOR_DATA` stabil sepanjang full run
