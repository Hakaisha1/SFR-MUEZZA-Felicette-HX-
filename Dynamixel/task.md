# Checklist Integrasi UART & Dynamixel (Sisi Movement)

Silakan berikan tanda centang (`[x]`) pada checklist di bawah ini ketika Anda (Tim Movement) sudah mengimplementasikannya di kode `main.c` / STM32:

## Tahap 1: Setup Parsing (Menerima Command)
- [ ] Tambahkan `#include "uart_protocol.h"` dan `#include "dynamixel_config.h"` di `main.c`.
- [ ] Buat variabel `uint8_t rx_byte_buffer;` di `main.c`.
- [ ] Panggil `HAL_UART_Receive_IT(&huartX, &rx_byte_buffer, 1);` satu kali di luar loop utama `while(1)`.
- [ ] Buat fungsi interupsi `HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)` (bisa di `main.c` atau `stm32f4xx_it.c`).
- [ ] Di dalam fungsi callback tersebut, panggil `UartProtocol_ParseByte(rx_byte_buffer);` lalu panggil ulang `HAL_UART_Receive_IT` agar bisa menerima byte berikutnya.

## Tahap 2: Logika Gerak (Reaksi terhadap Command)
Buat/implementasikan 3 fungsi callback berikut di `main.c`:
- [ ] `void UartProtocol_OnCmdGerak(const CmdGerakOmni_t* cmd) { ... }` 
      *(Gunakan `cmd->kecepatan_x` dst. untuk menyuplai nilai target kecepatan ke Gait Generator)*
- [ ] `void UartProtocol_OnCmdStateControl(const CmdStateControl_t* cmd) { ... }`
      *(Jika dapat perintah STATE_EMERGENCY_STOP, matikan/hentikan gerakan robot secara paksa)*
- [ ] `void UartProtocol_OnCmdManipulator(const CmdManipulator_t* cmd) { ... }`
      *(Gunakan untuk mengontrol capit/lengan)*

## Tahap 3: Membalas Telemetri
- [ ] Buat implementasi `void UartProtocol_TransmitBytes(uint8_t* data, uint16_t length)` di `main.c` yang memanggil `HAL_UART_Transmit`.
- [ ] Di dalam *while(1)* loop atau *Task FreeRTOS* yang berjalan periodik (misal tiap 50ms), pastikan Anda memanggil:
  - [ ] `UartProtocol_SendStatus(...)`
  - [ ] `UartProtocol_SendSensor(...)`

---
> Jika semua checklist ini sudah diselesaikan, STM32 siap 100% untuk dikomunikasikan dengan program Python (Integration) di Raspberry Pi!
