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

## 4. Timeline Mingguan (Contoh — 10 Minggu)

Sesuaikan jumlah minggu dengan deadline kompetisi kalian. Movement diberi start lebih awal & buffer lebih besar karena paling bergantung pada hardware fisik dan matematika kinematika.

### Minggu 1 — Define Interface & Setup
- **Semua**: diskusi protokol komunikasi, state machine, repo setup
- **Vision**: setup environment (OpenCV/YOLO), kumpulkan sample data awal (foto dummy & korban riil)
- **Movement**: pelajari/derive kinematika 1 kaki (3 DOF), setup komunikasi ke Dynamixel; pasang & verifikasi 4 sensor ultrasonik HC-SR04
- **Integration**: bikin skeleton UART RPi5↔STM32 dengan data dummy (belum ada logic asli)

### Minggu 2–3 — Core Development (Paralel)
- **Vision**:
  - Deteksi objek dasar (contour/color detection atau model ringan)
  - Klasifikasi dummy vs riil (warna + ada/tidak lengan)
  - Testing pakai video/gambar, belum di robot asli
- **Movement**:
  - Inverse kinematics 1 kaki → generalisasi ke 6 kaki
  - Implementasi gait dasar (tripod gait) untuk jalan lurus
  - Driver ultrasonik HC-SR04 (timer input-capture, 4 kanal bergiliran)
  - Testing fisik di hardware (paling banyak trial-and-error di sini)
- **Integration**:
  - Implementasi state machine dengan command placeholder
  - Watchdog/timeout untuk komunikasi

### Minggu 4 — Movement Lanjutan
- **Movement**:
  - Belok, mundur, penyesuaian gait di medan tidak rata (integrasi IMU)
  - Kontrol gripper/arm dasar
  - Obstacle reflex ultrasonik terbukti aktif (tanpa menunggu RPi5)
- **Vision**:
  - Tuning akurasi klasifikasi, mulai uji dengan variasi pencahayaan
- **Integration**:
  - Mulai sambungkan output vision → state machine (belum ke movement asli)

### Minggu 5 — Integrasi Tahap 1
- Sambungkan **vision → integration**: robot bisa "melihat" & mengklasifikasi target secara live
- **Movement**: finalisasi obstacle avoidance low-level pakai ultrasonik HC-SR04 (refleks cepat, tidak menunggu RPi5)
- Testing modul per modul masih terpisah, belum full pipeline

### Minggu 6 — Integrasi Tahap 2
- Sambungkan **integration → movement**: command asli mulai dikirim ke STM32
- Testing end-to-end sederhana: robot cari objek → dekati → klasifikasi
- Mulai catat bug & edge case (mis. false positive klasifikasi, gait tidak stabil saat belok mendekati target)

### Minggu 7 — Full Pipeline
- End-to-end penuh: **cari → dekati → klasifikasi → grip → bawa ke drop zone → lepas**
- Debugging bersama (integration memimpin sesi ini)
- Vision & Movement standby untuk perbaikan cepat sesuai bug yang ditemukan

### Minggu 8 — Robustness & Edge Case
- Handling kegagalan: gripper gagal, salah klasifikasi, robot nabrak/terjebak
- Failsafe di STM32 kalau komunikasi UART terputus
- Stress test: berbagai posisi target, jarak, sudut kamera
- Validasi noise ultrasonik (pantulan lantai/dinding, crosstalk antar sensor)

### Minggu 9 — Tuning & Optimasi
- Optimasi kecepatan gait vs stabilitas
- Optimasi kecepatan inferensi vision (FPS vs akurasi)
- Battery/power test saat semua servo aktif bersamaan

### Minggu 10 — Buffer & Simulasi Lomba
- Full run berulang kali meniru kondisi arena lomba
- Perbaikan minor & dokumentasi
- Cadangan waktu jika ada kendala hardware elektrik

---

## 5. Milestone Checklist

- [ ] Protokol komunikasi & state machine disepakati
- [ ] Vision: bisa klasifikasi dummy vs riil dengan akurasi memadai (offline test)
- [ ] Movement: robot bisa jalan lurus, belok, gripper berfungsi
- [ ] Movement: 4 sensor ultrasonik HC-SR04 terbaca & obstacle reflex aktif (< 8 cm)
- [ ] Integration: pipeline vision → decision → movement tersambung
- [ ] Full end-to-end run pertama berhasil (walau belum sempurna)
- [ ] Robustness: robot punya failsafe untuk kegagalan umum
- [ ] Tuning akhir & simulasi kondisi lomba

---

## 6. Catatan Manajemen Risiko

- **Movement** paling bergantung pada hardware fisik → prioritaskan akses ke robot untuk role ini di minggu-minggu awal.
- **Ultrasonik HC-SR04** rentan terhadap crosstalk jika 4 sensor ditrigger bersamaan → gunakan pola trigger bergiliran dengan jeda ≥ 20ms. Validasi juga pada permukaan dengan sudut ekstrem (lantai bertekstur, objek tipis) karena ultrasonik lebih sensitif terhadap sudut pantulan dibanding ToF.
- **Vision** butuh banyak data uji realistis → mulai kumpulkan foto/video kondisi arena sedini mungkin, jangan tunggu robot jadi.
- **Integration** akan menjadi titik debugging tersibuk di minggu 5–8 → alokasikan waktu ekstra & komunikasi intens dengan dua role lain.
- Sisakan minimal 1 minggu buffer sebelum deadline untuk kejutan teknis (servo rusak, baterai drop, dsb — ini ranah tim elektrik tapi berdampak ke testing software).