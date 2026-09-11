# Pre-Flash Safety Notes 🚀

Dokumen ini adalah ringkasan hasil pengecekan akhir (*Pre-Flash Audit*) sebelum program di-upload ke STM32F401 (BlackPill).

## 1. Status Kompilasi (Build)
- **Status:** `SUCCESS` (Berhasil tanpa error)
- **RAM Usage:** ~9.1% (Sangat aman)
- **Flash Usage:** ~11.7% (Sangat aman)
- Semua *library*, pengaturan PlatformIO, dan fungsi berhasil terhubung (*linked*) dengan benar.

## 2. Penghapusan Potensi *Bug* Kritis (UART Interrupt Collision)
Pada pengecekan terakhir, ditemukan satu potensi bug kritis:
- **Masalah:** Fungsi `HAL_UART_RxCpltCallback` masih aktif di `main.c` untuk membaca data dari `USART1` dan melemparkannya ke parser RPi5 (`UartProtocol_ParseByte`). Padahal `USART1` sekarang didedikasikan murni untuk komunikasi dua arah dengan **Dynamixel**. Jika Dynamixel membalas data (misal saat diping), data tersebut akan masuk ke *callback* ini dan mengacaukan mesin parser RPi5.
- **Solusi yang diterapkan:** Fungsi `HAL_UART_RxCpltCallback` di `main.c` **telah di-comment/dihapus** karena proses terima data dari RPi5 sudah dialihkan ke `CDC_Receive_FS` (USB CDC), dan driver Dynamixel (`dynamixel.c`) menggunakan *blocking mode* (`HAL_UART_Receive`) untuk menerima balasan sehingga tidak membutuhkan *callback* interrupt tersebut.

## 3. Pengingat Konfigurasi Hardware & ST-Link
- **Pin ST-Link:** Gunakan pin **PA13 (SWDIO)** dan **PA14 (SWCLK)**.
- **Tegangan:** Pastikan mencolokkan pin **3.3V** dari ST-Link, **JANGAN** menggunakan pin 5V langsung ke pin 3.3V STM32.
- ⚠️ **AWAS (STM32CubeMX):** Jika suatu saat Anda mengubah konfigurasi pin via aplikasi STM32CubeMX (`Closeloop.ioc`), **WAJIB** masuk ke menu **System Core -> SYS -> Debug** dan atur menjadi **Serial Wire**. Jika opsi ini terlewat (Disable), maka setelah STM32 menyala, pin PA13 & PA14 akan dialihfungsikan menjadi pin GPIO biasa, menyebabkan ST-Link tidak bisa lagi mendeteksi dan mem-flash STM32 (terkunci). Jika ini terjadi, Anda harus menekan dan menahan tombol *Reset* fisik di board sambil menekan tombol *Upload* di VSCode.

## 4. Rencana Iterasi Berikutnya (Untuk Tim Elektrik/Programmer)
- Mengaktifkan Hardware Timer PWM (misal `TIM2_CH1` di `PA0`) via CubeMX untuk Servo MG (Capit), lalu memasukkan kode `__HAL_TIM_SET_COMPARE` di `gripper.c`.
- Membuat rutin interupsi / *input capture* timer untuk 4 pin Echo Ultrasonik.
- Menambahkan kode polling ADC untuk mendeteksi voltase baterai.

---
**Kesimpulan:** Kode sudah **100% AMAN** untuk di-flash ke board sekarang. Silakan gunakan tombol *Upload* di PlatformIO!
