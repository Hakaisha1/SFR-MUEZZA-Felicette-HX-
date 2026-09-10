# Panduan Firmware & Arsitektur (Sisi Movement)

Dokumen ini berisi penjelasan mengenai arsitektur firmware STM32 untuk robot Hexapod (SFR-MUEZZA / Felicette HX), khususnya mengenai manajemen komunikasi dengan RPi5 dan kontrol servo Dynamixel.

## 🏗️ Arsitektur Hardware & Komunikasi
Berdasarkan konfigurasi terbaru, terdapat pembagian jalur komunikasi yang **krusial** agar tidak terjadi konflik data:

1. **Komunikasi RPi5 (Perintah & Telemetri)**
   - Menggunakan: **USB_DEVICE / CDC (Virtual COM Port)**
   - Pin: **PA11 (DM) & PA12 (DP)**
   - Penjelasan: STM32 akan terdeteksi sebagai serial port (`/dev/ttyACM0` atau sejenisnya) di Raspberry Pi 5. Kecepatannya tinggi (Full-Duplex USB FS).

2. **Komunikasi Motor Dynamixel**
   - Menggunakan: **USART1**
   - Mode: **Half-Duplex (Single Wire Mode)**
   - Pin: **PA9**
   - Penjelasan: Didedikasikan khusus untuk protokol Dynamixel (Daisy-chain ke 18 servo kaki & 2 servo capit).

---

## 🛠️ File Penting & Status Implementasi

### 1. `dynamixel_config.h`
Berisi mapping struct `DXL_ID_MAP` (18 ID servo kaki) dan `DXL_GRIPPER_IDS` (2 ID capit). Semua ID sudah dikonfigurasi dan sinkron dengan `main.c`.

### 2. `uart_protocol.c` / `.h`
Mesin parser untuk instruksi RPi5 dan fungsi pembungkus (Telemetri). Sudah diamankan dengan *XOR Checksum*.

### 3. Logika Transmit & Receive (Selesai Diimplementasikan)
Protokol telah dikaitkan ke USB CDC (bukan UART biasa). 
- **Receive:** Diletakkan pada fungsi `CDC_Receive_FS()` di `usbd_cdc_if.c`. Setiap byte yang masuk dari USB langsung diumpankan ke `UartProtocol_ParseByte()`.
- **Transmit:** Diletakkan pada fungsi `UartProtocol_TransmitBytes()` di `main.c`. Mengirim balik sensor dan status menggunakan `CDC_Transmit_FS()`.

### 4. Logika Gerak & Capit (Selesai Diimplementasikan)
Fungsi callback di `main.c`:
- `UartProtocol_OnCmdGerak`: Mengambil data pergerakan Omnidirectional dan melemparnya ke `gait_planner` (Tripod Gait).
- `UartProtocol_OnCmdStateControl`: Mematikan/menghidupkan torsi Dynamixel (Emergency Stop).
- `UartProtocol_OnCmdManipulator`: Target sudut (`sudut_lengan_1` & `2`) sudah diarahkan langsung ke `Gripper_Execute()` (menggunakan `pitch_deg` dan `yaw_deg`).

### 5. Pembacaan Sensor (TODO / Dummy)
Saat ini pengiriman telemetri ke RPi5 **sudah berjalan**, namun datanya **masih dummy/hardcoded** di dalam loop `while(1)` (sekitar baris 320 `main.c`).
Target yang harus diimplementasikan pada iterasi berikutnya:
- [ ] **Sensor Ultrasonik (HC-SR04)**: Harus dibuatkan kode pembacaan menggunakan *Timer Input Capture* atau *GPIO Polling*.
- [ ] **Baterai (Tegangan)**: Harus dibuatkan pembacaan ADC untuk mendeteksi low-battery.
- [ ] **IMU (Roll, Pitch)**: Harus dihubungkan lewat antarmuka I2C / SPI.
- [ ] **Error Hardware**: Diperbarui dengan membaca register alarm dari Dynamixel secara riil.

---
**Status Saat Ini:** Firmware **SIAP FLASH**. Anda bisa memprogram STM32 sekarang untuk memvalidasi gerakan kaki (kinematika) dan tes perintah dari Python/RPi5 via USB.

