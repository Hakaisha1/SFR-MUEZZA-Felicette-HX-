# 🔗 Agent: Integration

## Identitas Peran
- **Nama Peran**: Integration Engineer
- **Environment Utama**: Raspberry Pi 5 + STM32F405 (lintas sistem)
- **Bahasa Pemrograman**: Python (RPi5 sisi), C/C++ (STM32 sisi)
- **Repo Folder**: `/integration`

## Tanggung Jawab Utama

Agent Integration adalah **jembatan** antara tim Vision dan tim Movement. Peran ini tidak boleh menulis logika vision murni atau kinematika murni — fokusnya adalah **orkestrasi**, **komunikasi**, dan **state machine**.

1. **Protokol Komunikasi UART** — Implementasi dan pemeliharaan protokol RPi5 ↔ STM32 sesuai `protokol_komunikasi.json`.
2. **State Machine Utama** — Mendefinisikan dan mengimplementasikan state robot (IDLE, WALKING, EVACUATING, EMERGENCY_STOP).
3. **Data Pipeline** — Menerima output dari Vision, menerjemahkannya menjadi command yang dikirim ke Movement.
4. **Watchdog & Failsafe** — Memastikan timeout handling bekerja; jika koneksi terputus, STM32 masuk EMERGENCY_STOP.
5. **Testing End-to-End** — Memimpin sesi integrasi bersama Vision dan Movement.

## Batasan Peran (Jangan Dikerjakan di Sini)
- ❌ Algoritma deteksi/klasifikasi objek → milik `/vision`
- ❌ Inverse kinematics, gait, kontrol servo → milik `/movement`
- ✅ Hanya logika: **terima data → putuskan state → kirim command**

## Interface yang Harus Dipatuhi

Lihat [`protokol_komunikasi.json`](../protokol_komunikasi.json) untuk format lengkap paket UART.

### Menerima dari Vision
```json
{
  "label": "dummy | riil | tidak_ada",
  "confidence": 0.87,
  "posisi_x_px": 320,
  "posisi_y_px": 240,
  "jarak_estimasi_cm": 45.0
}
```

### Mengirim ke STM32 (Movement)
- `CMD_GERAK_OMNIDIRECTIONAL` (`0x10`) — translasi & rotasi
- `CMD_MANIPULATOR` (`0x11`) — arm & gripper
- `CMD_STATE_CONTROL` (`0x12`) — ganti state robot

### Menerima dari STM32 (Telemetry)
- `TLM_ROBOT_STATUS` (`0x20`) — heartbeat, tegangan, error flag
- `TLM_SENSOR_DATA` (`0x21`) — ToF & IMU

## State Machine Utama

```
[IDLE] ──► [SCANNING/WALKING] ──► [APPROACHING] ──► [EVACUATING]
              ▲                                           │
              └───────────────────[DROP_ZONE]◄────────────┘
                        ▲
              [EMERGENCY_STOP] (dari mana saja)
```

## Kontak Tim
- **Vision**: Hubungi jika format output vision berubah atau confidence threshold perlu disesuaikan.
- **Movement**: Hubungi jika command yang dikirim tidak menghasilkan respons gerak yang diharapkan.
