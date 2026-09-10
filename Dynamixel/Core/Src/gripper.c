/**
 * @file gripper.c
 * @brief Implementasi Kontrol Gripper 2-DOF (Pitch + Yaw) (C Port)
 * @author Movement Engineer
 * @date 2026-09-10
 */

#include "gripper.h"
#include <math.h>

#define MOVE_SPEED_DEG_PER_SEC 120.0f

static GripperState_t g_state = GRIPPER_STATE_IDLE;

static float g_current_pitch_deg = GRIPPER_PITCH_STANDBY_DEG;
static float g_current_yaw_deg   = GRIPPER_YAW_STANDBY_DEG;

static float g_target_pitch_deg = GRIPPER_PITCH_STANDBY_DEG;
static float g_target_yaw_deg   = GRIPPER_YAW_STANDBY_DEG;

static uint32_t g_last_update_tick_ms = 0;

static float clamp_pitch(float deg) {
    if (deg < GRIPPER_PITCH_MIN_DEG) return GRIPPER_PITCH_MIN_DEG;
    if (deg > GRIPPER_PITCH_MAX_DEG) return GRIPPER_PITCH_MAX_DEG;
    return deg;
}

static float clamp_yaw(float deg) {
    if (deg < GRIPPER_YAW_MIN_DEG) return GRIPPER_YAW_MIN_DEG;
    if (deg > GRIPPER_YAW_MAX_DEG) return GRIPPER_YAW_MAX_DEG;
    return deg;
}

static uint16_t deg_to_ax12_raw(float deg, uint16_t center) {
    float raw = (float)center + (deg / 0.29296875f);
    if (raw < 0.0f) raw = 0.0f;
    if (raw > 1023.0f) raw = 1023.0f;
    return (uint16_t)(raw + 0.5f);
}

void Gripper_Init(void) {
    g_current_pitch_deg = GRIPPER_PITCH_STANDBY_DEG;
    g_current_yaw_deg   = GRIPPER_YAW_STANDBY_DEG;
    g_target_pitch_deg  = GRIPPER_PITCH_STANDBY_DEG;
    g_target_yaw_deg    = GRIPPER_YAW_STANDBY_DEG;
    g_state = GRIPPER_STATE_IDLE;
}

void Gripper_Execute(GripperCommand_t cmd, GripperTarget_t target) {
    switch (cmd) {
        case GRIPPER_CMD_GRIP:
            g_target_pitch_deg = clamp_pitch(target.pitch_deg);
            g_target_yaw_deg   = clamp_yaw(target.yaw_deg);
            // TODO (Elektrik): Set PWM MG Servo untuk menutup capit (misal duty cycle 10%)
            g_state = GRIPPER_STATE_CLOSING;
            break;

        case GRIPPER_CMD_RELEASE:
            g_target_pitch_deg = clamp_pitch(target.pitch_deg);
            g_target_yaw_deg   = clamp_yaw(target.yaw_deg);
            // TODO (Elektrik): Set PWM MG Servo untuk membuka capit (misal duty cycle 5%)
            g_state = GRIPPER_STATE_RELEASING;
            break;

        case GRIPPER_CMD_SET_ANGLE:
            g_target_pitch_deg = clamp_pitch(target.pitch_deg);
            g_target_yaw_deg   = clamp_yaw(target.yaw_deg);
            g_state = GRIPPER_STATE_CLOSING;
            break;

        case GRIPPER_CMD_STANDBY:
            g_target_pitch_deg = GRIPPER_PITCH_STANDBY_DEG;
            g_target_yaw_deg   = GRIPPER_YAW_STANDBY_DEG;
            g_state = GRIPPER_STATE_RELEASING;
            break;

        case GRIPPER_CMD_NONE:
        default:
            break;
    }
}

bool Gripper_Update(uint32_t current_tick_ms) {
    if (g_state == GRIPPER_STATE_IDLE || g_state == GRIPPER_STATE_GRIPPING) {
        g_last_update_tick_ms = current_tick_ms;
        return true;
    }

    uint32_t dt_ms = current_tick_ms - g_last_update_tick_ms;
    if (dt_ms == 0) return false;
    g_last_update_tick_ms = current_tick_ms;

    float dt_sec = (float)dt_ms / 1000.0f;
    float max_step = MOVE_SPEED_DEG_PER_SEC * dt_sec;

    float pitch_diff = g_target_pitch_deg - g_current_pitch_deg;
    if (fabs(pitch_diff) > max_step) {
        g_current_pitch_deg += (pitch_diff > 0 ? max_step : -max_step);
    } else {
        g_current_pitch_deg = g_target_pitch_deg;
    }

    float yaw_diff = g_target_yaw_deg - g_current_yaw_deg;
    if (fabs(yaw_diff) > max_step) {
        g_current_yaw_deg += (yaw_diff > 0 ? max_step : -max_step);
    } else {
        g_current_yaw_deg = g_target_yaw_deg;
    }

    bool pitch_reached = (fabs(g_current_pitch_deg - g_target_pitch_deg) < 0.5f);
    bool yaw_reached   = (fabs(g_current_yaw_deg - g_target_yaw_deg) < 0.5f);

    if (pitch_reached && yaw_reached) {
        if (g_state == GRIPPER_STATE_CLOSING) {
            g_state = GRIPPER_STATE_GRIPPING;
        } else if (g_state == GRIPPER_STATE_RELEASING || g_state == GRIPPER_STATE_OPENING) {
            g_state = GRIPPER_STATE_IDLE;
        }
        return true;
    }

    return false;
}

GripperState_t Gripper_GetState(void) {
    return g_state;
}

GripperDxlPositions_t Gripper_GetDxlPositions(void) {
    GripperDxlPositions_t pos;
    pos.pitch_raw = deg_to_ax12_raw(g_current_pitch_deg, 512);
    pos.yaw_raw   = deg_to_ax12_raw(g_current_yaw_deg, 512);
    return pos;
}
