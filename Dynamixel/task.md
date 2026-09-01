# Checklist Integrasi UART & Dynamixel (Sisi Movement)

Silakan berikan tanda centang (`[x]`) pada checklist di bawah ini ketika Anda (Tim Movement) sudah mengimplementasikannya di kode `main.c` / STM32:

## Tahap 1: Setup Parsing (Menerima Command)
- [x] Tambahkan `#include "uart_protocol.h"` dan `#include "dynamixel_config.h"` di `main.c`.
- [x] Buat variabel `uint8_t rx_byte_buffer;` di `main.c`.
- [x] Panggil `HAL_UART_Receive_IT(&huartX, &rx_byte_buffer, 1);` satu kali di luar loop utama `while(1)`.
- [x] Buat fungsi interupsi `HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)` (bisa di `main.c` atau `stm32f4xx_it.c`).
- [x] Di dalam fungsi callback tersebut, panggil `UartProtocol_ParseByte(rx_byte_buffer);` lalu panggil ulang `HAL_UART_Receive_IT` agar bisa menerima byte berikutnya.

## Tahap 2: Logika Gerak (Reaksi terhadap Command)
Buat/implementasikan 3 fungsi callback berikut di `main.c`:
- [x] `void UartProtocol_OnCmdGerak(const CmdGerakOmni_t* cmd) { ... }` 
      *(Gunakan `cmd->kecepatan_x` dst. untuk menyuplai nilai target kecepatan ke Gait Generator)*
- [x] `void UartProtocol_OnCmdStateControl(const CmdStateControl_t* cmd) { ... }`
      *(Jika dapat perintah STATE_EMERGENCY_STOP, matikan/hentikan gerakan robot secara paksa)*
- [x] `void UartProtocol_OnCmdManipulator(const CmdManipulator_t* cmd) { ... }`
      *(Gunakan untuk mengontrol capit/lengan)*

## Tahap 3: Membalas Telemetri
- [x] Buat implementasi `void UartProtocol_TransmitBytes(uint8_t* data, uint16_t length)` di `main.c` yang memanggil `HAL_UART_Transmit`.
- [x] Di dalam *while(1)* loop atau *Task FreeRTOS* yang berjalan periodik (misal tiap 50ms), pastikan Anda memanggil:
  - [x] `UartProtocol_SendStatus(...)`
  - [x] `UartProtocol_SendSensor(...)`

---
> Jika semua checklist ini sudah diselesaikan, STM32 siap 100% untuk dikomunikasikan dengan program Python (Integration) di Raspberry Pi!
