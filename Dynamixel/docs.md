# Panduan Integrasi UART & Dynamixel (Sisi Movement)

Dokumen ini merupakan panduan bagi divisi Movement (STM32) untuk mengintegrasikan jembatan komunikasi (UART) dengan RPi5 dan manajemen ID servo tanpa perlu membuat parser dari nol.

## File Penting yang Sudah Disediakan
Tim Integration telah membuatkan 3 file di dalam `Core/Inc` dan `Core/Src` untuk mempermudah pekerjaan Anda:

1. **`dynamixel_config.h`**
   Berisi struct `DXL_ID_MAP` yang memetakan ID 18 servo kaki sesuai konfigurasi mekanis terbaru. Anda tidak perlu lagi mengingat ID secara manual (hindari *magic numbers*). Cukup gunakan `DXL_ID_MAP[LEG_KIRI_A].femur`.
   
2. **`uart_protocol.h`**
   Berisi struktur data (struct) untuk perintah (Command) dari RPi5 dan sensor (Telemetry) yang akan dikirim ke RPi5.

3. **`uart_protocol.c`**
   Mesin parser (Non-blocking State Machine) untuk memproses byte UART dari RPi5. Sudah dilengkapi dengan sistem **XOR Checksum** untuk menghindari robot merespons data yang terpotong/korup.

---

## 4 Langkah Implementasi di `main.c` Anda

Untuk membuat robot ini bisa menerima perintah dari RPi5 dan membalas dengan data sensor, ikuti 4 langkah berikut:

### 1. Hubungkan Parser ke Interrupt Penerima (RX)
Anda wajib menggunakan interupsi (bukan *polling*/`HAL_UART_Receive` biasa yang memblokir loop). Setiap 1 byte diterima, lemparkan byte itu ke fungsi parser.

Contoh di dalam `stm32f4xx_it.c` atau di fungsi callback HAL di `main.c`:
```c
#include "uart_protocol.h"

// Variabel penampung 1 byte
uint8_t rx_byte_buffer;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) { // Sesuaikan dengan UART yang mengarah ke RPi5
        // 1. Lemparkan byte ke parser
        UartProtocol_ParseByte(rx_byte_buffer);
        
        // 2. Aktifkan lagi mode interrupt untuk menunggu byte berikutnya
        HAL_UART_Receive_IT(huart, &rx_byte_buffer, 1); 
    }
}
```
*Pastikan Anda memanggil `HAL_UART_Receive_IT(&huart1, &rx_byte_buffer, 1);` satu kali di luar loop `while(1)` saat inisialisasi.*

### 2. Implementasi "Logika Gerak" (Fungsi Callback)
Jika paket yang masuk valid dan checksum-nya benar, `uart_protocol.c` akan otomatis memanggil salah satu dari 3 fungsi di bawah ini. Tugas Anda adalah **membuat definisi fungsi ini** di `main.c` atau `hexapod.c` dan mengisi apa yang harus robot lakukan.

```c
#include "uart_protocol.h"

// Menerima perintah kecepatan dari RPi5
void UartProtocol_OnCmdGerak(const CmdGerakOmni_t* cmd) {
    float vx = cmd->kecepatan_x;     // Maju / Mundur
    float vy = cmd->kecepatan_y;     // Geser Kiri / Kanan
    float yaw = cmd->kecepatan_yaw;  // Putar Kiri / Kanan
    
    // TODO: Masukkan vx, vy, yaw ini ke dalam Tripod Gait Generator Anda
}

// Menerima perintah perubahan State (Misal: Dipaksa berhenti)
void UartProtocol_OnCmdStateControl(const CmdStateControl_t* cmd) {
    if (cmd->state_target == STATE_EMERGENCY_STOP) {
        // TODO: Bekukan semua gerakan, matikan torsi jika perlu
    }
}

// Menerima perintah posisi Lengan/Capit
void UartProtocol_OnCmdManipulator(const CmdManipulator_t* cmd) {
    // TODO: Gerakkan capit sesuai cmd->status_gripper
}
```

### 3. Buat Fungsi Pengirim (Transmit)
Fungsi `uart_protocol.c` membutuhkan akses ke hardware UART Anda untuk bisa membalas data (Telemetri) ke RPi5. Tambahkan fungsi ini di `main.c`:

```c
void UartProtocol_TransmitBytes(uint8_t* data, uint16_t length) {
    // Sesuaikan &huart1 dengan port UART RPi5 Anda
    HAL_UART_Transmit(&huart1, data, length, 100);
}
```

### 4. Kirim Data Telemetri & Sensor Berkala
Di dalam loop utama `while(1)` (atau di dalam Task FreeRTOS), Anda wajib melaporkan status STM32 dan data ultrasonik/IMU secara berkala (misal setiap 50 milidetik).

```c
// 1. Kirim Status (Baterai, Error, State)
TlmRobotStatus_t status = {0};
status.state_sekarang = STATE_WALKING; // Ganti dengan state aktual
status.tegangan_baterai = 11.4;        // TODO: Baca dari ADC
status.status_hardware_error = 0;      // TODO: Cek error servo
UartProtocol_SendStatus(&status);

// 2. Kirim Sensor (Ultrasonik & IMU)
TlmSensorData_t sensor = {0};
sensor.us_depan_mm = 1500;  // TODO: Baca dari HC-SR04
sensor.us_kiri_mm = 9999;
// ... (isi yang lain)
UartProtocol_SendSensor(&sensor);
```

---
Dengan mengikuti 4 langkah di atas, integrasi komando dari AI (RPi5) ke kontrol mekanis (STM32) dipastikan aman dan sukses!
