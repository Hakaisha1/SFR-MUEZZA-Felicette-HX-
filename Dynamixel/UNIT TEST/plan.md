# Rencana Migrasi Hexapod SFR-MUEZZA ke Arduino Framework

## 1. Pendahuluan
Dokumen ini berisi panduan dan rencana terstruktur untuk melakukan migrasi sistem kontrol hexapod SFR-MUEZZA dari lingkungan STM32Cube (C) ke framework Arduino (C++) menggunakan PlatformIO (board `blackpill_f401cc`).

## 2. Pemetaan Hardware Final
Berdasarkan hasil analisis dan sinkronisasi, berikut adalah konfigurasi perangkat keras yang akan digunakan:

| Komponen | Koneksi / Pin | Keterangan Tambahan |
| :--- | :--- | :--- |
| **MCU Target** | STM32F401CCU6 | Board Blackpill |
| **Dynamixel Bus Kanan** | `PA9` (`Serial1`) | Komunikasi UART Half-Duplex |
| **Dynamixel Bus Kiri** | `PA2` (`Serial2`) | Komunikasi UART Half-Duplex |
| **Servo Capit (MG996R)**| `PB8` & `PB9` | PWM via `Servo.h` |
| **Ultrasonik (TRIG)** | `PB12` | 1 pin memicu ke-4 sensor bersamaan |
| **Ultrasonik (ECHO)** | `PA10` (Depan), `PB14` (Belakang), `PB13` (Kiri), `PA15` (Kanan) | |
| **Tombol / Saklar** | `PB3` | Input Digital |

## 3. Pemetaan ID Dynamixel
Sesuai dengan `dynamixel_id.md`, urutan ID yang digunakan adalah:
*   **Kaki Kiri (A, B, C):** `1, 2, 3` \| `4, 5, 6` \| `7, 8, 9`
*   **Kaki Kanan (A, B, C):** `10, 11, 12` \| `20, 21, 22` \| `30, 31, 32`
*   **Lengan (Manipulator):** Pitch (`0`), Yaw (`33`)

## 4. Aturan Pergerakan (Kinematika)
1.  **Batas Maksimal:** Semua kalkulasi sudut dari *Gait Planner* dan *Inverse Kinematics* harus melewati fungsi *clamping* berdasarkan batas minimum dan maksimum yang tertera pada `max_move.md`. Ini mencegah servo *stuck* karena batas mekanis.
2.  **Titik Berdiri (IDLE):** Derajat 0 pada perhitungan matematis akan dipetakan ke nilai **Center** masing-masing servo di `max_move.md` (contoh: Center Coxa Kiri C adalah `600`, bukan `512`).

## 5. Tahapan Eksekusi Migrasi

### Fase 1: Porting Algoritma Inti (Matematika)
*   Menyalin `kinematics.c` dan `gait_planner.c` dari folder utama.
*   Mengubah ekstensi menjadi `.cpp`.
*   Mengganti *include* STM32 HAL (`stm32f4xx_hal.h`) dengan `#include <Arduino.h>`.

### Fase 2: Pembuatan Driver IO (Arduino Style)
*   Membuat modul kontrol Dynamixel yang mendukung dua bus (`Serial1` dan `Serial2`).
*   Mengimplementasikan kontrol lengan dengan integrasi `Servo.h` untuk MG996R (`PB8` & `PB9`).
*   Membangun fungsi pembacaan Ultrasonik berbasis *interrupt* atau `pulseIn()` non-blocking, dengan sistem pemanggilan *trigger* tunggal pada `PB12`.

### Fase 3: Transisi dari FreeRTOS ke Non-Blocking Loop
*   Menggantikan tugas-tugas FreeRTOS (seperti `osDelay`) dengan arsitektur *State Machine* menggunakan `millis()`.
*   **Contoh:** *Gait update* dieksekusi tepat setiap 20ms tanpa memblokir pembacaan *serial* atau *sensor*.

### Fase 4: Komunikasi Telemetri (RPi5)
*   Mengaktifkan *build flag* `USBCON` untuk menggunakan pin USB MCU secara langsung (`Serial` murni).
*   Menyalin dan menyesuaikan `uart_protocol.c` menjadi sistem parsing data (JSON/Biner) yang lebih elegan dan terintegrasi dengan siklus Arduino.
