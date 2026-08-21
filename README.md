# Robot SAR Hexapod — Software Development Plan

Dokumen ini adalah rencana kerja tim software untuk robot SAR hexapod (evakuasi korban dummy vs riil). Tim dibagi menjadi 3 peran: **Vision**, **Movement**, **Integration**.

---

## 1. Struktur Tim

| Peran | Tanggung Jawab Utama | Environment |
|---|---|---|
| Vision | Deteksi & klasifikasi target (dummy/riil), estimasi posisi | Raspberry Pi 5 (Python) |
| Movement | Inverse kinematics kaki, gait, kontrol gripper, IMU, safety reflex | STM32F405 (C/C++) |
| Integration | Protokol komunikasi RPi5↔STM32, state machine, testing end-to-end | RPi5 + STM32 |

---

## 2. Stack Sensor Obstacle Avoidance

Robot menggunakan **4 sensor ultrasonik HC-SR04** (depan, belakang, kiri, kanan) untuk obstacle avoidance refleks — **tanpa sensor ToF (VL53L1X)**. Pembacaan diproses langsung di STM32 via timer input-capture, hasilnya dikirim ke RPi5 melalui telemetri UART.

| Posisi | Metode Baca STM32 | Threshold Reflex |
|---|---|---|
| Depan | TIM Input-Capture (ECHO pin) | < 8 cm → hentikan maju |
| Belakang | TIM Input-Capture (ECHO pin) | < 8 cm → hentikan mundur |
| Kiri | TIM Input-Capture (ECHO pin) | < 8 cm → hentikan geser kiri |
| Kanan | TIM Input-Capture (ECHO pin) | < 8 cm → hentikan geser kanan |

> ⚠️ Trigger 4 sensor secara bergiliran (bukan bersamaan) dengan jeda minimal 20ms antar trigger untuk menghindari crosstalk dan pantulan ganda.

---

## 3. Prasyarat Sebelum Mulai Coding

- [ ] Sepakati **protokol komunikasi** RPi5 ↔ STM32 (format command & telemetry, baud rate UART, checksum/error handling)
- [ ] Sepakati **state machine** utama robot (siapa pemegang keputusan di tiap state)
- [ ] Sepakati **struktur repo** (folder `/vision`, `/movement`, `/integration`, branch strategy)
- [ ] Definisikan **kontrak data**: format output vision (koordinat, label, confidence), format command movement (arah, sudut, grip)

> Fase ini idealnya selesai di **Minggu 1**, dikerjakan bertiga bersama sebelum masing-masing pecah kerja sendiri.

---

## 4. Milestone Roadmap

Progres diukur berdasarkan **pencapaian milestone**, bukan minggu. Setiap milestone punya syarat selesai yang jelas. Milestone harus diselesaikan **berurutan** — tidak boleh lompat ke milestone berikutnya jika milestone sebelumnya belum tercapai.

---

### 🏁 M0 — Setup & Kesepakatan Interface

> **Gerbang masuk**: Semua anggota sudah clone repo dan bisa build/run di environment masing-masing.

| Divisi | Deliverable | Syarat Selesai |
|---|---|---|
| **Semua** | Protokol komunikasi & state machine disepakati | `protokol_komunikasi.json`, `data_contract.json`, `state_machine.md` final dan di-review bertiga |
| **Semua** | Repo & branch strategy berjalan | Semua bisa push ke branch masing-masing, PR pertama berhasil merge |
| **Vision** | Environment siap | OpenCV/YOLO terinstall di RPi5, kamera terbaca stabil 640×480 |
| **Movement** | Environment siap | PlatformIO bisa build & flash ke STM32, 1 servo Dynamixel bisa digerakkan |
| **Integration** | Skeleton UART berjalan | RPi5 bisa kirim & terima paket dummy ke/dari STM32 tanpa error |

**Blocker jika belum selesai**: Tidak ada gunanya mulai coding fitur kalau interface belum disepakati — akan tabrakan nanti.

---

### 🏁 M1 — Fondasi Per Divisi (Paralel)

> **Gerbang masuk**: M0 selesai.

| Divisi | Deliverable | Syarat Selesai |
|---|---|---|
| **Vision** | Deteksi target dasar | Bounding box muncul di gambar/video uji offline, HSV atau model ringan |
| **Vision** | Klasifikasi dummy vs riil | Akurasi ≥ 85% pada dataset uji offline |
| **Vision** | Estimasi jarak | Error ≤ ±5 cm pada rentang 15–80 cm |
| **Movement** | Inverse Kinematics | IK 1 kaki tervalidasi (plot/simulasi), digeneralisasi ke 6 kaki |
| **Movement** | Tripod gait jalan lurus | Robot bisa jalan lurus stabil di lantai datar |
| **Movement** | Driver ultrasonik HC-SR04 | 4 sensor terbaca via TIM input-capture, trigger bergiliran, nilai jarak wajar |
| **Integration** | State machine skeleton | Transisi IDLE→WALKING→APPROACHING berjalan dengan data placeholder |
| **Integration** | Watchdog UART | Timeout 500ms terbukti aktif (cabut kabel → robot berhenti) |

**Catatan**: Setiap divisi bekerja paralel. Vision & Movement belum perlu terhubung ke robot secara penuh.

---

### 🏁 M2 — Movement Lanjutan

> **Gerbang masuk**: M1 Movement selesai (gait lurus + IK + ultrasonik).

| Divisi | Deliverable | Syarat Selesai |
|---|---|---|
| **Movement** | Gait belok & mundur | Robot bisa belok kiri/kanan dan mundur stabil |
| **Movement** | Integrasi IMU | Roll & pitch terbaca akurat ≤ ±2°, koreksi postur aktif di medan miring |
| **Movement** | Gripper/arm | Gripper Dynamixel + MG90S bisa buka/tutup terkontrol via command |
| **Movement** | Obstacle reflex aktif | Ultrasonik < 80mm → robot berhenti instan (tanpa menunggu RPi5) |
| **Vision** | Tuning klasifikasi | Diuji dalam variasi pencahayaan: terang, redup, backlit |
| **Integration** | Vision → state machine | Output Vision live terhubung ke state machine (belum ke Movement asli) |

---

### 🏁 M3 — Integrasi Awal

> **Gerbang masuk**: M1 semua divisi selesai, M2 Movement selesai.

| Divisi | Deliverable | Syarat Selesai |
|---|---|---|
| **Semua** | Vision → Integration terhubung | Robot bisa "melihat" & mengklasifikasi target secara live |
| **Semua** | Integration → Movement terhubung | Command asli dari state machine dikirim ke STM32, robot bergerak sesuai keputusan |
| **Semua** | End-to-end sederhana | Robot cari objek → dekati → klasifikasi (boleh belum grip) |

**Blocker jika belum selesai**: Jangan lanjut ke M4 jika pipeline dasar belum tersambung — semua fitur lanjutan tidak bisa diuji.

---

### 🏁 M4 — Full Pipeline

> **Gerbang masuk**: M3 selesai (pipeline dasar tersambung).

| Deliverable | Syarat Selesai |
|---|---|
| Full run end-to-end | **cari → dekati → klasifikasi → grip → bawa → drop → cari lagi** berhasil minimal 1x |
| Bug list terdokumentasi | Semua bug & edge case yang ditemukan dicatat (issue/dokumen) |
| Telemetri stabil | `TLM_ROBOT_STATUS` & `TLM_SENSOR_DATA` diterima RPi5 tanpa putus selama full run |

---

### 🏁 M5 — Robustness & Failsafe

> **Gerbang masuk**: M4 selesai (full pipeline pernah berhasil).

| Deliverable | Syarat Selesai |
|---|---|
| Handling gripper gagal | Robot retry gripping 2x, jika gagal → kembali SEARCHING, objek ditandai |
| Handling salah klasifikasi | Robot tidak infinite loop pada objek yang sama |
| Failsafe UART | Komunikasi terputus → EMERGENCY_STOP dalam 500ms |
| Validasi noise ultrasonik | Diuji dengan permukaan miring, lantai bertekstur, objek tipis — tidak false trigger |
| Stress test | Diuji dengan berbagai posisi target, jarak, sudut kamera, durasi panjang |

---

### 🏁 M6 — Tuning & Simulasi Lomba

> **Gerbang masuk**: M5 selesai (robot punya failsafe dan handling kegagalan).

| Deliverable | Syarat Selesai |
|---|---|
| Optimasi gait | Kecepatan vs stabilitas sudah dituning untuk kondisi arena |
| Optimasi vision | Inferensi ≤ 100ms/frame (10 FPS minimum), akurasi tetap ≥ 85% |
| Battery test | Full run dengan semua servo aktif, durasi cukup untuk sesi lomba |
| Simulasi lomba | Minimal **3x full run berturut-turut** meniru kondisi arena tanpa kegagalan fatal |
| Dokumentasi final | Semua dokumen up-to-date, tidak ada info yang outdated |

---

### Ringkasan Dependensi Antar Milestone

```
M0 (Setup & Kesepakatan)
 │
 ├── M1 Vision (paralel)──────┐
 ├── M1 Movement (paralel)────┤
 └── M1 Integration (paralel)─┤
                               │
                     M2 Movement Lanjutan
                               │
                         M3 Integrasi Awal
                               │
                         M4 Full Pipeline
                               │
                      M5 Robustness & Failsafe
                               │
                      M6 Tuning & Simulasi Lomba
```

---

## 5. Catatan Manajemen Risiko

- **Movement** paling bergantung pada hardware fisik → prioritaskan akses ke robot untuk role ini di milestone awal (M1–M2).
- **Ultrasonik HC-SR04** rentan terhadap crosstalk jika 4 sensor ditrigger bersamaan → gunakan pola trigger bergiliran dengan jeda ≥ 20ms. Validasi juga pada permukaan dengan sudut ekstrem (lantai bertekstur, objek tipis) karena ultrasonik lebih sensitif terhadap sudut pantulan.
- **Vision** butuh banyak data uji realistis → mulai kumpulkan foto/video kondisi arena sedini mungkin, jangan tunggu robot jadi.
- **Integration** akan menjadi titik debugging tersibuk di M3–M5 → alokasikan waktu ekstra & komunikasi intens dengan dua role lain.
- Sisakan waktu buffer sebelum deadline untuk kejutan teknis (servo rusak, baterai drop, dsb — ini ranah tim elektrik tapi berdampak ke testing software).