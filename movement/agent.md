# ⚙️ Agent: Movement

## Identitas Peran
- **Nama Peran**: Movement Engineer
- **Environment Utama**: STM32F405RGT6 (WeAct Black Pill)
- **Bahasa Pemrograman**: C / C++
- **Framework**: STM32CubeMX + PlatformIO
- **Repo Folder**: `/movement` (kode di-deploy ke folder `/Dynamixel` sebagai PlatformIO project)

## Tanggung Jawab Utama

Agent Movement bertanggung jawab atas **semua perilaku fisik robot** — dari satu sendi servo hingga pola berjalan kompleks. Tidak ada satu pun servo yang bergerak tanpa melalui kode di peran ini.

1. **Driver Servo Dynamixel** — Komunikasi serial TTL half-duplex ke 20–21 servo Dynamixel.
2. **Inverse Kinematics (IK) Kaki** — Menghitung sudut 3 servo per kaki (18 servo total) dari target posisi ujung kaki.
3. **Gait Generation** — Menghasilkan pola langkah (tripod, ripple) yang membuat robot berjalan stabil.
4. **Kontrol Keseimbangan** — Membaca IMU (MPU-9250/6500) dan mengkoreksi postur robot di medan tidak rata.
5. **Kontrol Gripper/Arm** — Menggerakkan 2–3 servo Dynamixel arm dan 1 servo MG90S (cengkeraman).
6. **Obstacle Avoidance Refleks** — Reaksi cepat terhadap data 4 sensor ToF (VL53L1X) via TCA9548A — **tidak menunggu perintah RPi5**.
7. **Penerima Command UART** — Menerima dan mengeksekusi paket command dari Integration (RPi5).

## Batasan Peran (Jangan Dikerjakan di Sini)
- ❌ Logika "harus pergi ke mana" berdasarkan kamera → milik `/integration`
- ❌ Pemrosesan gambar → milik `/vision`
- ✅ Hanya logika: **terima command → hitung gerak → gerakkan servo + safety reflex lokal**

## Interface yang Harus Dipatuhi

Lihat [`protokol_komunikasi.json`](../protokol_komunikasi.json) untuk format lengkap paket UART.

### Menerima dari RPi5 (Integration)
| ID Pesan | Nama | Aksi |
|---|---|---|
| `0x10` | `CMD_GERAK_OMNIDIRECTIONAL` | Set kecepatan translasi (x, y) dan rotasi (yaw) |
| `0x11` | `CMD_MANIPULATOR` | Set sudut arm dan status gripper |
| `0x12` | `CMD_STATE_CONTROL` | Ganti state (IDLE, WALKING, dll) |

### Mengirim ke RPi5 (Telemetry)
| ID Pesan | Nama | Isi |
|---|---|---|
| `0x20` | `TLM_ROBOT_STATUS` | State, tegangan baterai, bitmask error |
| `0x21` | `TLM_SENSOR_DATA` | ToF (4 arah) + IMU (roll, pitch) |

## Arsitektur Kode STM32

```
Interrupt UART RX  ──► Parser Paket ──► Command Queue
                                              │
FreeRTOS Task: MovementTask ◄────────────────┘
       │
       ├── GaitGenerator (gait pattern)
       ├── InverseKinematics (per kaki)
       ├── DynamixelDriver (kirim posisi)
       ├── IMUReader (MPU-9250 via SPI/I2C)
       ├── ToFReader (VL53L1X via TCA9548A I2C)
       └── ObstacleReflex (keputusan lokal, prioritas tertinggi)
```

## Failsafe Lokal (Wajib Ada)

Movement **harus** mengimplementasikan failsafe berikut **tanpa menunggu RPi5**:

1. **Timeout UART**: Jika tidak ada paket command masuk selama **500ms** → paksa masuk `EMERGENCY_STOP`.
2. **Obstacle Reflex**: Jika ToF depan < 8 cm → hentikan gerak maju secara instan, terlepas dari command yang sedang aktif.
3. **Over-Temperature**: Jika ada servo melaporkan suhu ≥ 65°C → hentikan robot, kirim error flag ke RPi5.

## Kontak Tim
- **Integration**: Hubungi jika format paket UART perlu disesuaikan atau ada kebutuhan data telemetry baru.
- **Vision**: Tidak ada kontak langsung dari Movement ke Vision.
