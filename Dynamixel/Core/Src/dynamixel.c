#include "dynamixel.h"

static UART_HandleTypeDef *dxl_huart;

void Dxl_Init(UART_HandleTypeDef *huart) {
    dxl_huart = huart;
}

static void Dxl_TransmitPacket(uint8_t id, uint8_t inst, uint8_t *params, uint8_t param_len) {
    uint8_t length = param_len + 2;
    uint8_t packet[256];
    
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = inst;
    
    uint8_t checksum = id + length + inst;
    for (uint8_t i = 0; i < param_len; i++) {
        packet[5 + i] = params[i];
        checksum += params[i];
    }
    packet[5 + param_len] = ~checksum;
    
    HAL_UART_Transmit(dxl_huart, packet, length + 4, HAL_MAX_DELAY);
}

void Dxl_Write8(uint8_t id, uint8_t reg, uint8_t data) {
    uint8_t params[2] = {reg, data};
    Dxl_TransmitPacket(id, DXL_INST_WRITE, params, 2);
}

void Dxl_Write16(uint8_t id, uint8_t reg, uint16_t data) {
    uint8_t params[3] = {reg, (uint8_t)(data & 0xFF), (uint8_t)((data >> 8) & 0xFF)};
    Dxl_TransmitPacket(id, DXL_INST_WRITE, params, 3);
}

bool Dxl_Ping(uint8_t id) {
    uint8_t packet[6];
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = 0x02; // Length
    packet[4] = DXL_INST_PING; // 0x01
    
    uint8_t checksum = id + 0x02 + DXL_INST_PING;
    packet[5] = ~checksum;
    
    // Clear overrun flag in case there's old data
    __HAL_UART_CLEAR_OREFLAG(dxl_huart);
    
    // Transmit
    HAL_UART_Transmit(dxl_huart, packet, 6, 10);
    
    // Receive Status Packet (6 bytes)
    uint8_t rx_packet[6] = {0};
    HAL_StatusTypeDef res = HAL_UART_Receive(dxl_huart, rx_packet, 6, 20); // 20ms timeout
    
    if (res == HAL_OK) {
        if (rx_packet[0] == 0xFF && rx_packet[1] == 0xFF && rx_packet[2] == id) {
            return true; // ID found!
        }
    }
    return false;
}

void Dxl_TorqueEnable(uint8_t id, bool enable) {
    Dxl_Write8(id, DXL_REG_TORQUE_ENABLE, enable ? 1 : 0);
}

void Dxl_SetGoalPosition(uint8_t id, uint16_t position) {
    Dxl_Write16(id, DXL_REG_GOAL_POSITION, position);
}

void Dxl_SetGoalPositionAndSpeed(uint8_t id, uint16_t position, uint16_t speed) {
    uint8_t params[5] = {
        DXL_REG_GOAL_POSITION,
        (uint8_t)(position & 0xFF),
        (uint8_t)((position >> 8) & 0xFF),
        (uint8_t)(speed & 0xFF),
        (uint8_t)((speed >> 8) & 0xFF)
    };
    Dxl_TransmitPacket(id, DXL_INST_WRITE, params, 5);
}

void Dxl_SyncWritePosition(Dxl_SyncWriteData_t *data, uint8_t num_servos) {
    uint8_t param_len = (num_servos * 3) + 2;
    uint8_t params[256];
    
    params[0] = DXL_REG_GOAL_POSITION;
    params[1] = 2; // Data length per servo (Goal Position is 2 bytes)
    
    uint8_t idx = 2;
    for (uint8_t i = 0; i < num_servos; i++) {
        params[idx++] = data[i].id;
        params[idx++] = (uint8_t)(data[i].position & 0xFF);
        params[idx++] = (uint8_t)((data[i].position >> 8) & 0xFF);
    }
    
    Dxl_TransmitPacket(DXL_BROADCAST_ID, DXL_INST_SYNC_WRITE, params, param_len);
}

void Dxl_SimpleTest(uint8_t id) {
    // 1. Nyalakan Torsi (agar servo mengunci posisi dan bisa bergerak)
    Dxl_TorqueEnable(id, true);
    
    // 2. Variabel statis untuk menyimpan status
    static uint8_t toggle = 0;
    toggle = !toggle;
    
    // 3. Nyalakan/Matikan LED berdasarkan status toggle
    Dxl_Write8(id, DXL_REG_LED, toggle);
    
    // 4. Bergerak bolak-balik antara posisi 200 dan 800 (dari total 0-1023)
    // 512 adalah posisi tengah (150 derajat)
    if (toggle) {
        Dxl_SetGoalPosition(id, 200);
    } else {
        Dxl_SetGoalPosition(id, 800);
    }
}

// Note: Read functions (Dxl_Read8, Dxl_Read16) require UART Receive implementation 
// which typically uses Interrupts or DMA for half-duplex communication timing.
// For basic walking, transmit-only (Write) is often sufficient.
uint8_t Dxl_Read8(uint8_t id, uint8_t reg) {
    // To be implemented: Send READ_DATA instruction and wait for status packet.
    return 0;
}

uint16_t Dxl_Read16(uint8_t id, uint8_t reg) {
    // To be implemented: Send READ_DATA instruction and wait for status packet.
    return 0;
}
