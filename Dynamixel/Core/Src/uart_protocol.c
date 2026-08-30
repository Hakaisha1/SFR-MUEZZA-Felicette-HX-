#include "uart_protocol.h"
#include <string.h> // Untuk memcpy

// Maksimal payload yang diperbolehkan di sistem
#define MAX_PAYLOAD_SIZE 32

typedef enum {
    RX_STATE_WAIT_HEADER_1 = 0,
    RX_STATE_WAIT_HEADER_2,
    RX_STATE_WAIT_LENGTH,
    RX_STATE_WAIT_MSG_ID,
    RX_STATE_READ_PAYLOAD,
    RX_STATE_WAIT_CHECKSUM
} UartRxState_t;

// Variabel State Machine (Static agar persisten antar panggilan)
static UartRxState_t rx_state = RX_STATE_WAIT_HEADER_1;
static uint8_t rx_length = 0;
static uint8_t rx_msg_id = 0;
static uint8_t rx_payload[MAX_PAYLOAD_SIZE];
static uint8_t rx_payload_index = 0;
static uint8_t rx_calculated_checksum = 0;

/**
 * @brief Helper untuk reset state machine ke awal.
 */
static void UartProtocol_ResetRxState(void) {
    rx_state = RX_STATE_WAIT_HEADER_1;
    rx_length = 0;
    rx_msg_id = 0;
    rx_payload_index = 0;
    rx_calculated_checksum = 0;
}

/**
 * @brief Dispatcher setelah 1 paket valid diterima.
 */
static void UartProtocol_ProcessPacket(void) {
    switch (rx_msg_id) {
        case CMD_GERAK_OMNIDIRECTIONAL:
            if (rx_length == sizeof(CmdGerakOmni_t)) {
                CmdGerakOmni_t cmd;
                memcpy(&cmd, rx_payload, sizeof(CmdGerakOmni_t));
                UartProtocol_OnCmdGerak(&cmd);
            }
            break;

        case CMD_MANIPULATOR:
            if (rx_length == sizeof(CmdManipulator_t)) {
                CmdManipulator_t cmd;
                memcpy(&cmd, rx_payload, sizeof(CmdManipulator_t));
                UartProtocol_OnCmdManipulator(&cmd);
            }
            break;

        case CMD_STATE_CONTROL:
            if (rx_length == sizeof(CmdStateControl_t)) {
                CmdStateControl_t cmd;
                memcpy(&cmd, rx_payload, sizeof(CmdStateControl_t));
                UartProtocol_OnCmdStateControl(&cmd);
            }
            break;
            
        default:
            // Message ID tidak dikenal, abaikan
            break;
    }
}

/**
 * @brief Parser byte masuk (Non-blocking state machine).
 */
void UartProtocol_ParseByte(uint8_t byte) {
    switch (rx_state) {
        case RX_STATE_WAIT_HEADER_1:
            if (byte == UART_HEADER_1) {
                UartProtocol_ResetRxState(); // Pastikan bersih
                rx_state = RX_STATE_WAIT_HEADER_2;
            }
            break;

        case RX_STATE_WAIT_HEADER_2:
            if (byte == UART_HEADER_2) {
                rx_state = RX_STATE_WAIT_LENGTH;
            } else {
                UartProtocol_ResetRxState();
            }
            break;

        case RX_STATE_WAIT_LENGTH:
            if (byte <= MAX_PAYLOAD_SIZE) {
                rx_length = byte;
                rx_state = RX_STATE_WAIT_MSG_ID;
            } else {
                // Length melanggar batas maksimal
                UartProtocol_ResetRxState();
            }
            break;

        case RX_STATE_WAIT_MSG_ID:
            rx_msg_id = byte;
            rx_calculated_checksum = byte; // Mulai checksum (XOR MSG_ID)
            
            if (rx_length > 0) {
                rx_state = RX_STATE_READ_PAYLOAD;
            } else {
                // Tidak ada payload (misal ping command dll di masa depan)
                rx_state = RX_STATE_WAIT_CHECKSUM;
            }
            break;

        case RX_STATE_READ_PAYLOAD:
            rx_payload[rx_payload_index++] = byte;
            rx_calculated_checksum ^= byte; // XOR tiap byte payload
            
            if (rx_payload_index >= rx_length) {
                rx_state = RX_STATE_WAIT_CHECKSUM;
            }
            break;

        case RX_STATE_WAIT_CHECKSUM:
            if (byte == rx_calculated_checksum) {
                // Checksum OK, proses payload
                UartProtocol_ProcessPacket();
            }
            // Setelah selesai (sukses atau gagal checksum), kembali tunggu header
            UartProtocol_ResetRxState();
            break;

        default:
            UartProtocol_ResetRxState();
            break;
    }
}

/**
 * @brief Mengirim paket telemetri umum ke UART (Helper function).
 */
static void UartProtocol_SendPacket(uint8_t msg_id, uint8_t* payload, uint8_t length) {
    uint8_t buffer[64];
    if (length + 5 > sizeof(buffer)) return; // Cek batas buffer
    
    buffer[0] = UART_HEADER_1;
    buffer[1] = UART_HEADER_2;
    buffer[2] = length;
    buffer[3] = msg_id;
    
    uint8_t checksum = msg_id;
    for (uint8_t i = 0; i < length; i++) {
        buffer[4 + i] = payload[i];
        checksum ^= payload[i];
    }
    
    buffer[4 + length] = checksum;
    
    // Panggil implementasi hardware user
    UartProtocol_TransmitBytes(buffer, length + 5);
}

void UartProtocol_SendStatus(const TlmRobotStatus_t* status) {
    UartProtocol_SendPacket(TLM_ROBOT_STATUS, (uint8_t*)status, sizeof(TlmRobotStatus_t));
}

void UartProtocol_SendSensor(const TlmSensorData_t* sensor) {
    UartProtocol_SendPacket(TLM_SENSOR_DATA, (uint8_t*)sensor, sizeof(TlmSensorData_t));
}
