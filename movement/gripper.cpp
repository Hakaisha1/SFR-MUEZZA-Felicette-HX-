/**
 * @file gripper.cpp
 * @brief Implementasi Kontrol Gripper 2-DOF (Pitch + Yaw) via Dynamixel AX-12A
 * @author Movement Engineer
 * @date 2026-09-08
 */

#include "gripper.h"
#include <cmath>

namespace Movement {

// ============================================================
// Constructor & Init
// ============================================================

GripperController::GripperController()
    : state_(GripperState::IDLE),
      current_pitch_deg_(GRIPPER_PITCH_STANDBY_DEG),
      current_yaw_deg_(GRIPPER_YAW_STANDBY_DEG),
      target_pitch_deg_(GRIPPER_PITCH_STANDBY_DEG),
      target_yaw_deg_(GRIPPER_YAW_STANDBY_DEG),
      last_update_tick_ms_(0)
{
}

void GripperController::init() {
    current_pitch_deg_ = GRIPPER_PITCH_STANDBY_DEG;
    current_yaw_deg_   = GRIPPER_YAW_STANDBY_DEG;
    target_pitch_deg_  = GRIPPER_PITCH_STANDBY_DEG;
    target_yaw_deg_    = GRIPPER_YAW_STANDBY_DEG;
    state_ = GripperState::IDLE;
}

// ============================================================
// Execute Command
// ============================================================

void GripperController::execute(GripperCommand cmd, const GripperTarget& target) {
    switch (cmd) {
        case GripperCommand::GRIP:
            // Turunkan pitch (arahkan ke bawah) dan tutup yaw (jepit)
            target_pitch_deg_ = clamp_pitch(-45.0f);   // Arahkan ke bawah
            target_yaw_deg_   = clamp_yaw(GRIPPER_YAW_MAX_DEG);  // Tutup penuh
            state_ = GripperState::CLOSING;
            break;

        case GripperCommand::RELEASE:
            // Buka yaw terlebih dahulu, lalu naikkan pitch
            target_yaw_deg_   = clamp_yaw(GRIPPER_YAW_MIN_DEG);  // Buka penuh
            target_pitch_deg_ = clamp_pitch(0.0f);     // Kembali horizontal
            state_ = GripperState::RELEASING;
            break;

        case GripperCommand::SET_ANGLE:
            // Set sudut manual sesuai parameter
            target_pitch_deg_ = clamp_pitch(target.pitch_deg);
            target_yaw_deg_   = clamp_yaw(target.yaw_deg);
            state_ = GripperState::CLOSING; // Generic moving state
            break;

        case GripperCommand::STANDBY:
            target_pitch_deg_ = GRIPPER_PITCH_STANDBY_DEG;
            target_yaw_deg_   = GRIPPER_YAW_STANDBY_DEG;
            state_ = GripperState::RELEASING;
            break;

        case GripperCommand::NONE:
        default:
            break;
    }
}

// ============================================================
// Update Loop (Gradual Movement)
// ============================================================

bool GripperController::update(uint32_t current_tick_ms) {
    if (state_ == GripperState::IDLE || state_ == GripperState::GRIPPING) {
        last_update_tick_ms_ = current_tick_ms;
        return true; // Sudah di posisi target
    }

    // Hitung delta time
    uint32_t dt_ms = current_tick_ms - last_update_tick_ms_;
    if (dt_ms == 0) return false;
    last_update_tick_ms_ = current_tick_ms;

    float dt_sec = static_cast<float>(dt_ms) / 1000.0f;
    float max_step = MOVE_SPEED_DEG_PER_SEC * dt_sec;

    // Gerakkan pitch menuju target secara gradual
    float pitch_diff = target_pitch_deg_ - current_pitch_deg_;
    if (std::fabs(pitch_diff) > max_step) {
        current_pitch_deg_ += (pitch_diff > 0 ? max_step : -max_step);
    } else {
        current_pitch_deg_ = target_pitch_deg_;
    }

    // Gerakkan yaw menuju target secara gradual
    float yaw_diff = target_yaw_deg_ - current_yaw_deg_;
    if (std::fabs(yaw_diff) > max_step) {
        current_yaw_deg_ += (yaw_diff > 0 ? max_step : -max_step);
    } else {
        current_yaw_deg_ = target_yaw_deg_;
    }

    // Cek apakah sudah sampai di target
    bool pitch_reached = (std::fabs(current_pitch_deg_ - target_pitch_deg_) < 0.5f);
    bool yaw_reached   = (std::fabs(current_yaw_deg_ - target_yaw_deg_) < 0.5f);

    if (pitch_reached && yaw_reached) {
        // Transisi state berdasarkan aksi sebelumnya
        if (state_ == GripperState::CLOSING) {
            state_ = GripperState::GRIPPING;
        } else if (state_ == GripperState::RELEASING || state_ == GripperState::OPENING) {
            state_ = GripperState::IDLE;
        }
        return true;
    }

    return false;
}

// ============================================================
// Output Posisi Raw Dynamixel
// ============================================================

GripperDxlPositions GripperController::get_dxl_positions() const {
    GripperDxlPositions pos;
    pos.pitch_raw = deg_to_ax12_raw(current_pitch_deg_);
    pos.yaw_raw   = deg_to_ax12_raw(current_yaw_deg_);
    return pos;
}

// ============================================================
// Konversi & Utilitas
// ============================================================

uint16_t GripperController::deg_to_ax12_raw(float deg, uint16_t center) {
    // AX-12A: 0–1023 (0°–300°), resolusi 0.29296875°/unit
    // Center (150° / 512) = posisi netral
    float raw = static_cast<float>(center) + (deg / 0.29296875f);
    if (raw < 0.0f) raw = 0.0f;
    if (raw > 1023.0f) raw = 1023.0f;
    return static_cast<uint16_t>(raw + 0.5f);
}

float GripperController::clamp_pitch(float deg) {
    if (deg < GRIPPER_PITCH_MIN_DEG) return GRIPPER_PITCH_MIN_DEG;
    if (deg > GRIPPER_PITCH_MAX_DEG) return GRIPPER_PITCH_MAX_DEG;
    return deg;
}

float GripperController::clamp_yaw(float deg) {
    if (deg < GRIPPER_YAW_MIN_DEG) return GRIPPER_YAW_MIN_DEG;
    if (deg > GRIPPER_YAW_MAX_DEG) return GRIPPER_YAW_MAX_DEG;
    return deg;
}

} // namespace Movement
