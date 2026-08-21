# State Machine — Robot SAR Hexapod

Dokumen ini mendefinisikan seluruh state (kondisi/mode kerja) robot, syarat perpindahan antar state, serta siapa (RPi5/STM32) yang bertanggung jawab di tiap state. Dibuat supaya Vision, Movement, dan Integration punya pemahaman yang sama sebelum mulai coding.

> 📋 Semua threshold dan format data yang disebutkan di sini merujuk ke [`data_contract.json`](data_contract.json). Paket UART yang dikirim antar RPi5↔STM32 mengikuti [`protokol_komunikasi.json`](protokol_komunikasi.json).

---

## 1. Prinsip Dasar

- **RPi5 (Integration/Vision) = pemegang keputusan** — yang menentukan robot pindah dari satu state ke state lain via command `CMD_STATE_CONTROL` (`0x12`).
- **STM32 (Movement) = eksekutor + safety reflex** — menjalankan perintah dan punya hak override darurat (misal berhenti otomatis kalau **ultrasonik HC-SR04 depan** mendeteksi halangan < 8 cm), meski sedang diperintah bergerak.
- Robot **hanya boleh berada di 1 state pada satu waktu**.
- Setiap perpindahan state harus punya **syarat yang jelas dan terukur** (bukan asumsi).

---

## 2. Dua Layer State Machine

State machine robot memiliki **dua layer** yang saling terhubung:

### Layer 1 — State STM32 (Low-Level, via `CMD_STATE_CONTROL` 0x12)

Ini adalah state yang **STM32 kenali** sesuai [`protokol_komunikasi.json`](protokol_komunikasi.json):

| Nilai | Nama State STM32 | Deskripsi |
|---|---|---|
| `0` | `IDLE` | Servo aktif, robot diam, siap menerima command |
| `1` | `WALKING` | Robot bergerak sesuai `CMD_GERAK_OMNIDIRECTIONAL` (`0x10`) |
| `2` | `EVACUATING` | Robot dalam proses evakuasi (grip/bawa/lepas) |
| `3` | `EMERGENCY_STOP` | Semua servo berhenti, posisi mengunci |

### Layer 2 — Sub-State RPi5 (High-Level, logika di Integration)

Di dalam RPi5, Integration menjalankan logika yang **lebih detail**. Setiap sub-state dipetakan ke salah satu state STM32 di atas:

| Sub-State RPi5 | State STM32 yang Dikirim | Deskripsi |
|---|---|---|
| `SEARCHING` | `WALKING` (1) | Menjelajahi arena mencari target |
| `APPROACHING` | `WALKING` (1) | Mendekati target yang terdeteksi |
| `CLASSIFYING` | `IDLE` (0) | Berhenti, konfirmasi dummy/riil |
| `GRIPPING` | `EVACUATING` (2) | Memposisikan dan menutup gripper |
| `CARRYING` | `EVACUATING` (2) | Membawa korban ke drop zone |
| `RELEASING` | `EVACUATING` (2) | Melepas gripper di drop zone |
| `DROP_DONE` | `IDLE` (0) | Selesai drop, siap cari lagi |
| `ERROR_RETRY` | `IDLE` (0) | Menangani kegagalan |

> 💡 **Kenapa dua layer?** STM32 tidak perlu tahu detail "lagi cari korban" atau "lagi klasifikasi" — dia hanya perlu tahu: "jalan", "evakuasi", atau "diam". Logika keputusan detail ada di RPi5.

---

## 3. Diagram Alur State (Layer RPi5)

```
                ┌─────────────────┐
        ┌──────▶│    SEARCHING    │◀──────────────────────┐
        │       │ STM32: WALKING  │                       │
        │       └────────┬────────┘                       │
        │                │ confidence ≥ 0.75              │
        │                ▼                                │
        │       ┌─────────────────┐                       │
        │       │   APPROACHING   │                       │
        │       │ STM32: WALKING  │                       │
        │       └────────┬────────┘                       │
        │                │ jarak_estimasi_cm < 15          │
        │                ▼                                │
        │       ┌─────────────────┐                       │
        │       │   CLASSIFYING   │                       │
        │       │  STM32: IDLE    │                       │
        │       └────────┬────────┘                       │
        │  label=dummy   │   │ label=riil                 │
        └────────────────┘   ▼                            │
                    ┌─────────────────┐                   │
                    │    GRIPPING     │                   │
                    │STM32: EVACUATING│                   │
                    └────────┬────────┘                   │
           gagal capit       │   │ berhasil capit          │
           (N percobaan)     │   ▼                         │
                ┌────────┐  │ ┌─────────────────┐         │
                │ ERROR  │  │ │    CARRYING     │         │
                │ RETRY  │  │ │STM32: EVACUATING│         │
                │STM32:  │  │ └────────┬────────┘         │
                │ IDLE   │  │          │ sampai drop zone  │
                └───┬────┘  │          ▼                   │
                    │       │ ┌─────────────────┐          │
                    │       │ │   RELEASING     │          │
                    │       │ │STM32: EVACUATING│          │
                    │       │ └────────┬────────┘          │
                    │       │          └───────────────────┘
                    └───────┘

     ┌──────────────────────────────────────────────────────┐
     │  EMERGENCY_STOP (STM32: 3) — dari state manapun     │
     │  Dipicu oleh: ultrasonik < 80mm, timeout UART,      │
     │  servo overtemp ≥ 65°C, atau perintah manual        │
     └──────────────────────────────────────────────────────┘
```

---

## 4. Definisi Tiap Sub-State

### `SEARCHING`
- **Deskripsi**: Robot berjalan menjelajahi arena mencari objek yang mirip korban.
- **STM32 state**: `WALKING` (1) — RPi5 kirim `CMD_GERAK_OMNIDIRECTIONAL` (`0x10`) dengan pola eksplorasi.
- **Input yang dipantau**: hasil deteksi kamera (Vision output), data ultrasonik HC-SR04 (hindari tabrakan, otomatis di STM32).
- **Syarat keluar**: Vision mendeteksi objek dengan `confidence ≥ 0.75` → pindah ke `APPROACHING`.
- **Command UART tipikal**:
  ```python
  # Maju 0.3 m/s, tidak crab walk, sedikit rotasi untuk scanning
  struct.pack('<fff', 0.3, 0.0, 0.1)  # CMD 0x10
  ```

### `APPROACHING`
- **Deskripsi**: Robot bergerak mendekati objek yang terdeteksi, menyesuaikan arah berdasarkan `posisi_x_px` dari Vision.
- **STM32 state**: `WALKING` (1) — RPi5 kirim `CMD_GERAK_OMNIDIRECTIONAL` (`0x10`) dengan koreksi arah.
- **Syarat keluar**: `jarak_estimasi_cm < 15` (dari Vision output) → pindah ke `CLASSIFYING`.
- **Syarat gagal**: objek hilang dari kamera > 2 detik (timeout) → kembali ke `SEARCHING`.

### `CLASSIFYING`
- **Deskripsi**: Robot **berhenti**, Vision mengambil beberapa frame untuk konfirmasi dummy atau korban riil.
- **STM32 state**: `IDLE` (0) — RPi5 kirim `CMD_STATE_CONTROL` (`0x12`, state_target=0).
- **Syarat keluar**:
  - Jika `label = dummy` (mayoritas dari N frame) → kembali ke `SEARCHING`
  - Jika `label = riil` (mayoritas dari N frame) → lanjut ke `GRIPPING`
- **Catatan**: Ambil minimal **5 frame** dan voting mayoritas untuk kurangi false positive.

### `GRIPPING`
- **Deskripsi**: Robot memposisikan gripper dan mencapit korban riil.
- **STM32 state**: `EVACUATING` (2) — RPi5 kirim `CMD_MANIPULATOR` (`0x11`).
- **Command UART tipikal**:
  ```python
  # Turunkan arm, tutup gripper kuat (untuk korban riil)
  struct.pack('<ffB', 45.0, 90.0, 1)  # CMD 0x11, status_gripper=1
  ```
- **Syarat keluar**:
  - Berhasil capit (feedback servo dari `TLM_ROBOT_STATUS` 0x20, tidak ada error) → pindah ke `CARRYING`
  - Gagal capit setelah **2 percobaan** → pindah ke `ERROR_RETRY`

### `CARRYING`
- **Deskripsi**: Robot membawa korban riil menuju drop zone/titik evakuasi.
- **STM32 state**: `EVACUATING` (2) — RPi5 kirim `CMD_GERAK_OMNIDIRECTIONAL` (`0x10`) menuju drop zone.
- **Syarat keluar**: Robot mencapai drop zone (marker visual, atau jarak tempuh estimasi) → pindah ke `RELEASING`.

### `RELEASING`
- **Deskripsi**: Robot melepas gripper di drop zone.
- **STM32 state**: `EVACUATING` (2) — RPi5 kirim `CMD_MANIPULATOR` (`0x11`).
- **Command UART tipikal**:
  ```python
  # Buka gripper
  struct.pack('<ffB', 0.0, 0.0, 0)  # CMD 0x11, status_gripper=0
  ```
- **Syarat keluar**: Gripper terbuka (feedback servo) → kembali ke `SEARCHING`.

### `ERROR_RETRY`
- **Deskripsi**: Menangani kegagalan (gripper gagal, robot terjebak).
- **STM32 state**: `IDLE` (0) — robot berhenti dulu.
- **Aksi**: Retry terbatas (misal ulangi gripping 2x). Jika tetap gagal → kembali ke `SEARCHING` dan tandai objek ini sudah pernah dicoba (supaya tidak infinite loop).

### `EMERGENCY_STOP`
- **Deskripsi**: Semua gerak berhenti, servo mengunci di posisi aman.
- **STM32 state**: `EMERGENCY_STOP` (3).
- **Dipicu oleh** (otomatis di STM32, **tanpa menunggu RPi5**):
  - Ultrasonik HC-SR04 depan < 80 mm (8 cm)
  - Timeout UART > 500 ms (tidak ada command dari RPi5)
  - Servo overtemperature ≥ 65°C
- **Juga bisa dipicu manual** oleh RPi5 via `CMD_STATE_CONTROL` (`0x12`, state_target=3).

---

## 5. Tabel Ringkas Transisi

| Dari | Kondisi | Ke | Command ke STM32 |
|---|---|---|---|
| SEARCHING | `confidence ≥ 0.75` | APPROACHING | `0x10` (ubah arah) |
| APPROACHING | `jarak_estimasi_cm < 15` | CLASSIFYING | `0x12` (state=0, IDLE) |
| APPROACHING | objek hilang > 2 detik | SEARCHING | `0x10` (lanjut eksplorasi) |
| CLASSIFYING | `label = dummy` | SEARCHING | `0x12` (state=1, WALKING) |
| CLASSIFYING | `label = riil` | GRIPPING | `0x12` (state=2) + `0x11` |
| GRIPPING | berhasil capit | CARRYING | `0x10` (menuju drop zone) |
| GRIPPING | gagal 2x | ERROR_RETRY | `0x12` (state=0, IDLE) |
| CARRYING | sampai drop zone | RELEASING | `0x11` (buka gripper) |
| RELEASING | gripper terbuka | SEARCHING | `0x12` (state=1, WALKING) |
| ERROR_RETRY | retry / habis percobaan | SEARCHING | `0x12` (state=1, WALKING) |
| *(any)* | ultrasonik < 80mm / timeout / overtemp | EMERGENCY_STOP | *(otomatis STM32)* |

---

## 6. Safety Reflex (Tanggung Jawab STM32, di Luar State Machine RPi5)

Beberapa hal **tidak boleh menunggu keputusan RPi5** karena butuh reaksi milidetik:

| Kondisi | Sensor | Aksi STM32 | Threshold |
|---|---|---|---|
| Halangan depan | Ultrasonik HC-SR04 depan | Hentikan gerak maju instan | < 80 mm (8 cm) |
| Halangan belakang | Ultrasonik HC-SR04 belakang | Hentikan gerak mundur | < 80 mm |
| Halangan kiri/kanan | Ultrasonik HC-SR04 kiri/kanan | Hentikan geser arah tersebut | < 80 mm |
| Timeout komunikasi | Internal timer | Paksa `EMERGENCY_STOP` | > 500 ms tanpa command |
| Servo kepanasan | Feedback Dynamixel | Paksa `EMERGENCY_STOP`, kirim error flag | ≥ 65°C |
| Robot miring ekstrem | IMU (MPU-9250) | Koreksi gait / berhenti | Roll/pitch > threshold |

Safety reflex ini berjalan **paralel** di STM32, tidak peduli robot sedang di state apa. Hasilnya dilaporkan ke RPi5 via `TLM_ROBOT_STATUS` (`0x20`, bit `status_hardware_error`).

> ⚠️ Ultrasonik HC-SR04 dibaca secara bergiliran (bukan serentak) dengan jeda ≥ 20ms antar trigger untuk menghindari crosstalk. Lihat [`data_contract.json`](data_contract.json) → `frekuensi_trigger_ultrasonik_per_sensor_ms`.

---

## 7. Hal yang Perlu Disepakati Bertiga Sebelum Coding

Nilai default sudah didefinisikan di [`data_contract.json`](data_contract.json), tapi perlu divalidasi bersama:

- [ ] Confidence minimum untuk `SEARCHING → APPROACHING` (default: **0.75**)
- [ ] Jarak threshold untuk `APPROACHING → CLASSIFYING` (default: **15 cm**)
- [ ] Jumlah frame voting untuk klasifikasi di `CLASSIFYING` (saran: **5 frame**)
- [ ] Jumlah retry maksimum di `GRIPPING` sebelum masuk `ERROR_RETRY` (saran: **2x**)
- [ ] Bagaimana cara mendeteksi "sudah sampai drop zone" (marker visual? odometry?)
- [ ] Format pesan telemetry STM32 → RPi5 sudah sesuai `TLM_ROBOT_STATUS` (`0x20`) dan `TLM_SENSOR_DATA` (`0x21`)

---

## 8. Catatan untuk Pemula

- State machine ini **cukup digambar di kertas dulu** dan disepakati bertiga sebelum ada satu baris kode ditulis.
- Setiap kotak (state) di diagram = satu kondisi yang jelas kapan mulai dan kapan berakhir.
- Kalau nemu kasus yang "diagramnya nggak cukup" (misal robot lagi CARRYING tapi kamera lihat korban riil lain), itu tandanya perlu didiskusikan bertiga dan ditambahkan sebagai aturan baru — jangan diputuskan sendiri oleh satu role saja, karena akan berdampak ke semua modul.