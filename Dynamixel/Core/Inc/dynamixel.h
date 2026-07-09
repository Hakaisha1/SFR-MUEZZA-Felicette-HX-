#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* Dynamixel Protocol 1.0 Instructions */
#define DXL_INST_PING           0x01
#define DXL_INST_READ           0x02
#define DXL_INST_WRITE          0x03
#define DXL_INST_REG_WRITE      0x04
#define DXL_INST_ACTION         0x05
#define DXL_INST_RESET          0x06
#define DXL_INST_SYNC_WRITE     0x83

/* AX-12A Registers (EEPROM & RAM) */
#define DXL_REG_ID              3
#define DXL_REG_BAUD_RATE       4
#define DXL_REG_RETURN_DELAY    5
#define DXL_REG_CW_ANGLE_LIMIT  6
#define DXL_REG_CCW_ANGLE_LIMIT 8
#define DXL_REG_TORQUE_ENABLE   24
#define DXL_REG_LED             25
#define DXL_REG_GOAL_POSITION   30
#define DXL_REG_MOVING_SPEED    32
#define DXL_REG_TORQUE_LIMIT    34
#define DXL_REG_PRESENT_POS     36
#define DXL_REG_PRESENT_SPEED   38
#define DXL_REG_PRESENT_LOAD    40
#define DXL_REG_PRESENT_VOLT    42
#define DXL_REG_PRESENT_TEMP    43
#define DXL_REG_MOVING          46

/* Constants */
#define DXL_BROADCAST_ID        0xFE

/* Struct for SyncWrite of Goal Position */
typedef struct {
    uint8_t id;
    uint16_t position;
} Dxl_SyncWriteData_t;

/* Initialization */
// Pass the UART handle that will communicate with the dynamixels.
void Dxl_Init(UART_HandleTypeDef *huart);

/* Core Functions */
void Dxl_Write8(uint8_t id, uint8_t reg, uint8_t data);
void Dxl_Write16(uint8_t id, uint8_t reg, uint16_t data);
uint8_t Dxl_Read8(uint8_t id, uint8_t reg);
uint16_t Dxl_Read16(uint8_t id, uint8_t reg);

/* Helper Functions */
void Dxl_TorqueEnable(uint8_t id, bool enable);
void Dxl_SetGoalPosition(uint8_t id, uint16_t position);
void Dxl_SetGoalPositionAndSpeed(uint8_t id, uint16_t position, uint16_t speed);
void Dxl_SimpleTest(uint8_t id); // Fungsi tes untuk pergerakan bolak-balik

/* Sync Write */
// Send position to multiple servos simultaneously (very useful for hexapod gait)
void Dxl_SyncWritePosition(Dxl_SyncWriteData_t *data, uint8_t num_servos);

#endif // DYNAMIXEL_H
