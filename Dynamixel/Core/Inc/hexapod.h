#ifndef HEXAPOD_H
#define HEXAPOD_H

#include "dynamixel.h"
#include "kinematics.h"
#include "gait_planner.h"
#include "cmsis_os.h" // FreeRTOS definitions for STM32

/* Servo ID Mapping (Example Configuration) */
// Format: Leg Number, Joint
// Leg 0: Right Front, Leg 1: Left Front
// Leg 2: Right Middle, Leg 3: Left Middle
// Leg 4: Right Back, Leg 5: Left Back
#define DXL_ID_L0_COXA  1
#define DXL_ID_L0_FEMUR 2
#define DXL_ID_L0_TIBIA 3

#define DXL_ID_L1_COXA  4
#define DXL_ID_L1_FEMUR 5
#define DXL_ID_L1_TIBIA 6
// ... map rest of 18 servos accordingly

void Hexapod_Init(UART_HandleTypeDef *huart);
void Hexapod_StartTasks(void);

// High level control
void Hexapod_SetVelocity(float x_speed, float y_speed, float rot_speed);

#endif // HEXAPOD_H
