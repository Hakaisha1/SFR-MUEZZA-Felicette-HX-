/**
 * @file gait.cpp
 * @brief Implementasi Tripod Gait Generator untuk Robot SAR Hexapod pada STM32
 * @author Movement Engineer
 * @date 2026-08-22
 */

#include "gait.h"
#include <cmath>
#include <algorithm>

namespace Movement {

TripodGaitGenerator::TripodGaitGenerator(const KinematicsSolver& solver, const GaitConfig& config)
    : ik_solver_(solver),
      config_(config),
      state_(GaitState::IDLE),
      last_tick_ms_(0),
      cycle_start_tick_ms_(0),
      cycle_progress_(0.0f),
      vx_scale_(1.0f),
      vy_scale_(0.0f) {

    // Konfigurasi tripod:
    // Group 0 (Tripod A): 0 (RF), 3 (LM), 4 (RB)
    // Group 1 (Tripod B): 1 (LF), 2 (RM), 5 (LB)
    leg_group_assignment_[0] = 0; // RF
    leg_group_assignment_[1] = 1; // LF
    leg_group_assignment_[2] = 1; // RM
    leg_group_assignment_[3] = 0; // LM
    leg_group_assignment_[4] = 0; // RB
    leg_group_assignment_[5] = 1; // LB

    init();
}

void TripodGaitGenerator::init() {
    state_ = GaitState::IDLE;
    cycle_progress_ = 0.0f;
    cycle_start_tick_ms_ = 0;
    last_tick_ms_ = 0;

    // Set posisi netral berdiri untuk seluruh 6 kaki (koordinat lokal coxa)
    for (uint8_t i = 0; i < NUM_LEGS; ++i) {
        neutral_poses_[i] = Vector3D(config_.neutral_reach_x, 0.0f, config_.neutral_height_z);
        current_targets_[i] = neutral_poses_[i];
    }

    // Hitung posisi sudut awal saat standby
    ik_solver_.compute_all_legs_ik(current_targets_, current_joint_angles_);
}

void TripodGaitGenerator::start() {
    if (state_ == GaitState::IDLE || state_ == GaitState::STOPPING) {
        state_ = GaitState::WALKING;
        // Inisialisasi awal siklus
        cycle_progress_ = 0.0f;
    }
}

void TripodGaitGenerator::stop() {
    if (state_ == GaitState::WALKING || state_ == GaitState::STARTING) {
        state_ = GaitState::STOPPING;
    }
}

void TripodGaitGenerator::emergency_freeze() {
    state_ = GaitState::IDLE;
}

void TripodGaitGenerator::set_step_config(float stride_length_mm, float step_height_mm, uint32_t period_ms) {
    config_.stride_length = stride_length_mm;
    config_.step_height = step_height_mm;
    if (period_ms >= 200) {
        config_.cycle_period_ms = period_ms;
    }
}

void TripodGaitGenerator::set_velocity(float vx_ratio, float vy_ratio) {
    vx_scale_ = std::fmax(-1.0f, std::fmin(1.0f, vx_ratio));
    vy_scale_ = std::fmax(-1.0f, std::fmin(1.0f, vy_ratio));
}

LegPhase TripodGaitGenerator::get_leg_phase(uint8_t leg_index) const {
    if (leg_index >= NUM_LEGS || state_ == GaitState::IDLE) {
        return LegPhase::STANCE;
    }

    uint8_t group = leg_group_assignment_[leg_index];
    float leg_phase = std::fmod(cycle_progress_ + group * 0.5f, 1.0f);

    // 0.0 .. 0.5 adalah fase SWING, 0.5 .. 1.0 adalah fase STANCE
    return (leg_phase < 0.5f) ? LegPhase::SWING : LegPhase::STANCE;
}

Vector3D TripodGaitGenerator::calculate_foot_trajectory(uint8_t leg_index, float normalized_phase) const {
    const Vector3D& neutral = neutral_poses_[leg_index];
    
    if (state_ == GaitState::IDLE) {
        return neutral;
    }

    uint8_t group = leg_group_assignment_[leg_index];
    // Offset fasa 0.5 antar grup A dan B
    float leg_phase = std::fmod(normalized_phase + group * 0.5f, 1.0f);

    float effective_stride_x = config_.stride_length * vx_scale_;
    float effective_stride_y = config_.stride_width + (config_.stride_length * vy_scale_);
    float half_stride_x = effective_stride_x * 0.5f;
    float half_stride_y = effective_stride_y * 0.5f;

    Vector3D target = neutral;

    if (leg_phase < 0.5f) {
        // ==========================================
        // FASE 1: SWING (Kaki terangkat melangkah maju)
        // Sub-fase progression: s in [0.0 .. 1.0]
        // ==========================================
        float s = leg_phase * 2.0f; // rentang 0.0 sampai 1.0

        // Lintasan horizontal X dan Y menggunakan smooth cosine interpolation (mengurangi sentakan servo)
        float smooth_s = 0.5f * (1.0f - std::cos(s * PI_F));
        float offset_x = -half_stride_x + (smooth_s * effective_stride_x);
        float offset_y = -half_stride_y + (smooth_s * effective_stride_y);

        // Lintasan vertikal Z menggunakan kurva sinusoidal arc lift
        float offset_z = config_.step_height * std::sin(s * PI_F);

        target.x = neutral.x + offset_x;
        target.y = neutral.y + offset_y;
        target.z = neutral.z + offset_z;

    } else {
        // ==========================================
        // FASE 2: STANCE (Kaki menapak mendorong robot)
        // Sub-fase progression: s in [0.0 .. 1.0]
        // ==========================================
        float s = (leg_phase - 0.5f) * 2.0f; // rentang 0.0 sampai 1.0

        // Lintasan linear horizontal ke belakang dengan kecepatan kontak tanah konstan
        float offset_x = half_stride_x - (s * effective_stride_x);
        float offset_y = half_stride_y - (s * effective_stride_y);

        target.x = neutral.x + offset_x;
        target.y = neutral.y + offset_y;
        target.z = neutral.z; // Tetap kokoh menapak di tanah
    }

    return target;
}

bool TripodGaitGenerator::update(uint32_t current_tick_ms) {
    if (cycle_start_tick_ms_ == 0) {
        cycle_start_tick_ms_ = current_tick_ms;
        last_tick_ms_ = current_tick_ms;
    }

    // Jika idle, pastikan kembali ke pose netral
    if (state_ == GaitState::IDLE) {
        for (uint8_t i = 0; i < NUM_LEGS; ++i) {
            current_targets_[i] = neutral_poses_[i];
        }
        return ik_solver_.compute_all_legs_ik(current_targets_, current_joint_angles_);
    }

    // Hitung kemajuan siklus waktu (non-blocking)
    uint32_t elapsed_ms = current_tick_ms - cycle_start_tick_ms_;
    float raw_progress = static_cast<float>(elapsed_ms % config_.cycle_period_ms) / 
                         static_cast<float>(config_.cycle_period_ms);

    // Cek jika sedang dalam proses STOPPING dan siklus selesai
    if (state_ == GaitState::STOPPING) {
        if (elapsed_ms >= config_.cycle_period_ms) {
            state_ = GaitState::IDLE;
            for (uint8_t i = 0; i < NUM_LEGS; ++i) {
                current_targets_[i] = neutral_poses_[i];
            }
            return ik_solver_.compute_all_legs_ik(current_targets_, current_joint_angles_);
        }
    }

    cycle_progress_ = raw_progress;
    last_tick_ms_ = current_tick_ms;

    // Hitung posisi target untuk seluruh 6 kaki
    for (uint8_t i = 0; i < NUM_LEGS; ++i) {
        current_targets_[i] = calculate_foot_trajectory(i, cycle_progress_);
    }

    // Selesaikan Inverse Kinematics untuk 18 servo
    return ik_solver_.compute_all_legs_ik(current_targets_, current_joint_angles_);
}

void TripodGaitGenerator::get_sync_write_positions(uint16_t out_dxl_positions[TOTAL_SERVOS]) const {
    uint8_t idx = 0;
    for (uint8_t i = 0; i < NUM_LEGS; ++i) {
        out_dxl_positions[idx++] = current_joint_angles_.legs[i].coxa_to_ax12();
        out_dxl_positions[idx++] = current_joint_angles_.legs[i].femur_to_ax12();
        out_dxl_positions[idx++] = current_joint_angles_.legs[i].tibia_to_ax12();
    }
}

} // namespace Movement
