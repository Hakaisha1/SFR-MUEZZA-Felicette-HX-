/**
 * @file gripper.h
 * @brief Kontrol Gripper 2-DOF (Pitch + Yaw) via Dynamixel AX-12A (C Port)
 * @author Movement Engineer
 * @date 2026-09-10
 */

#ifndef GRIPPER_H
#define GRIPPER_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================
// Batas Sudut Gripper (derajat, relatif terhadap center=150°)
// ============================================================
#define GRIPPER_PITCH_MIN_DEG -60.0f
#define GRIPPER_PITCH_MAX_DEG  60.0f
#define GRIPPER_YAW_MIN_DEG    0.0f
#define GRIPPER_YAW_MAX_DEG    90.0f

#define GRIPPER_PITCH_STANDBY_DEG 0.0f
#define GRIPPER_YAW_STANDBY_DEG   0.0f

// ============================================================
// State & Commands
// ============================================================

typedef enum {
    GRIPPER_STATE_IDLE = 0,
    GRIPPER_STATE_OPENING,
    GRIPPER_STATE_CLOSING,
    GRIPPER_STATE_GRIPPING,
    GRIPPER_STATE_RELEASING,
    GRIPPER_STATE_ERROR
} GripperState_t;

typedef enum {
    GRIPPER_CMD_NONE = 0,
    GRIPPER_CMD_GRIP,
    GRIPPER_CMD_RELEASE,
    GRIPPER_CMD_SET_ANGLE,
    GRIPPER_CMD_STANDBY
} GripperCommand_t;

typedef struct {
    float pitch_deg;
    float yaw_deg;
} GripperTarget_t;

typedef struct {
    uint16_t pitch_raw;
    uint16_t yaw_raw;
} GripperDxlPositions_t;

// ============================================================
// Public Functions
// ============================================================

void Gripper_Init(void);
void Gripper_Execute(GripperCommand_t cmd, GripperTarget_t target);
bool Gripper_Update(uint32_t current_tick_ms);

GripperState_t Gripper_GetState(void);
GripperDxlPositions_t Gripper_GetDxlPositions(void);

#endif // GRIPPER_H
