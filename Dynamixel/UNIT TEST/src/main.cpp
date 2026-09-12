#include <Arduino.h>

HardwareSerial dxl(PA9);
const uint32_t DXL_BAUD = 1000000;

UART_HandleTypeDef* huart_dxl;

bool dxlPing(uint8_t id) {
  uint8_t packet[6];
  packet[0] = 0xFF;
  packet[1] = 0xFF;
  packet[2] = id;
  packet[3] = 0x02; // Length
  packet[4] = 0x01; // PING
  
  uint8_t checksum = id + 0x02 + 0x01;
  packet[5] = ~checksum;
  
  // Bersihkan error flag bawaan hardware
  __HAL_UART_CLEAR_OREFLAG(huart_dxl);
  
  // Transmit menggunakan Native HAL (Persis seperti CubeMX)
  HAL_UART_Transmit(huart_dxl, packet, 6, 10);
  
  // Receive Status Packet (6 bytes) menggunakan Native HAL
  uint8_t rx_packet[6] = {0};
  HAL_StatusTypeDef res = HAL_UART_Receive(huart_dxl, rx_packet, 6, 20); // 20ms timeout
  
  if (res == HAL_OK) {
      if (rx_packet[0] == 0xFF && rx_packet[1] == 0xFF && rx_packet[2] == id) {
          return true;
      }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  
  while (!Serial) {
    delay(10);
  }
  
  // Inisialisasi UART dari Arduino
  dxl.setHalfDuplex();
  dxl.begin(DXL_BAUD);
  
  // Ambil handle Native HAL dari Arduino untuk kita bypass
  huart_dxl = dxl.getHandle();
  
  // MATIKAN INTERRUPT ARDUINO AGAR TIDAK BENTROK DENGAN HAL_UART_Receive
  __HAL_UART_DISABLE_IT(huart_dxl, UART_IT_RXNE);
  
  delay(1000); 
  Serial.println("\n======================================");
  Serial.println("Program Scanner Dynamixel (HAL BYPASS)");
  Serial.println("======================================");
}

void loop() {
  uint8_t foundCount = 0;
  
  Serial.println("\n--- Memulai Scanning (0-253) ---");
  for (uint8_t id = 0; id <= 253; id++) {
    if (dxlPing(id)) {
      foundCount++;
      Serial.print("Servo Ditemukan: ID ");
      Serial.println(id);
    }
  }

  if (foundCount > 0) {
    Serial.print("Scan selesai. Total servo terdeteksi: ");
    Serial.println(foundCount);
  } else {
    Serial.println("TIDAK ADA SERVO TERDETEKSI! Cek kabel & power.");
  }

  Serial.println("Menunggu 3 detik sebelum scan ulang...\n");
  delay(3000); 
}
