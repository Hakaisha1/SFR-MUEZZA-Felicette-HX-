# 📖 Guide: Integration

Panduan teknis pengembangan untuk folder `/integration`. Baca `agent.md` terlebih dahulu sebelum dokumen ini.

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

Gunakan pola berikut sebagai dasar `state_machine.py`:

```python
from enum import Enum

class State(Enum):
    IDLE = 0
    WALKING = 1
    APPROACHING = 2
    EVACUATING = 3
    DROP_ZONE = 4
    EMERGENCY_STOP = 5

class StateMachine:
    def __init__(self):
        self.state = State.IDLE

    def update(self, data_vision, data_telemetry):
        """Dipanggil setiap loop. Return command untuk dikirim ke STM32."""
        if data_telemetry.get('hardware_error'):
            self.state = State.EMERGENCY_STOP
            return self._command_stop()

        if self.state == State.IDLE:
            self.state = State.WALKING
        elif self.state == State.WALKING:
            if data_vision['label'] in ('dummy', 'riil') and data_vision['confidence'] > 0.75:
                self.state = State.APPROACHING
        elif self.state == State.APPROACHING:
            if data_vision['jarak_estimasi_cm'] < 15:
                self.state = State.EVACUATING
        # ... tambahkan transisi lainnya

    def _command_stop(self):
        return {'msg_id': 0x12, 'state_target': 3}
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

## Checklist Integrasi Bertahap

- [ ] Skeleton UART dapat kirim dan terima data dummy tanpa error
- [ ] Checksum berfungsi — paket korup berhasil dideteksi dan diabaikan
- [ ] Watchdog terbukti aktif: cabut kabel UART → robot berhenti dalam 500ms
- [ ] State machine IDLE → WALKING → APPROACHING tersambung ke output vision live
- [ ] State machine EVACUATING → DROP_ZONE berjalan end-to-end
- [ ] Full pipeline: deteksi korban → dekati → grip → letakkan di zona drop
