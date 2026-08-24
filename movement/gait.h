/**
 * @file gait.h
 * @brief Tripod Gait Generator untuk Robot SAR Hexapod (STM32 C++)
 * @author Movement Engineer
 * @date 2026-08-22
 */

#ifndef MOVEMENT_GAIT_H
#define MOVEMENT_GAIT_H

#include "kinematics.h"
#include <stdint.h>

namespace Movement {

/**
 * @brief Fase siklus pergerakan satu kaki
 */
enum class LegPhase : uint8_t {
    STANCE = 0,  ///< Kaki menapak di tanah dan mendorong bodi robot
    SWING = 1    ///< Kaki terangkat di udara dan melangkah ke depan
};

/**
 * @brief State operasional gait engine
 */
enum class GaitState : uint8_t {
    IDLE = 0,     ///< Robot diam di posisi standby/netral
    STARTING,     ///< Transisi dari standby ke langkah pertama
    WALKING,      ///< Berjalan stabil berulang (cyclic tripod)
    STOPPING      ///< Transisi berhenti dan kembali ke posisi netral
};

/**
 * @brief Konfigurasi parameter langkah tripod gait
 */
struct GaitConfig {
    float stride_length;       ///< Panjang langkah maju/mundur (mm), default 40.0f
    float stride_width;        ///< Panjang langkah geser lateral (mm), default 0.0f
    float step_height;         ///< Ketinggian angkat kaki saat swing (mm), default 25.0f
    uint32_t cycle_period_ms;  ///< Durasi 1 siklus langkah penuh (ms), default 800ms
    float neutral_reach_x;     ///< Jarak horizontal kaki dari pangkal coxa saat berdiri (mm), default 130.0f
    float neutral_height_z;    ///< Ketinggian bodi robot (z negatif ke bawah, mm), default -85.0f

    constexpr GaitConfig(
        float stride_len = 40.0f,
        float stride_w = 0.0f,
        float step_h = 25.0f,
        uint32_t period_ms = 800,
        float reach_x = 130.0f,
        float height_z = -85.0f
    ) : stride_length(stride_len),
        stride_width(stride_w),
        step_height(step_h),
        cycle_period_ms(period_ms),
        neutral_reach_x(reach_x),
        neutral_height_z(height_z) {}
};

/**
 * @brief Kelas Tripod Gait Generator untuk hexapod 6 kaki
 */
class TripodGaitGenerator {
public:
    TripodGaitGenerator(const KinematicsSolver& solver, const GaitConfig& config = GaitConfig());

    /**
     * @brief Inisialisasi posisi awal robot (standby pose)
     */
    void init();

    /**
     * @brief Mulai pergerakan tripod walking
     */
    void start();

    /**
     * @brief Hentikan pergerakan (robot akan menyelesaikan siklus dan kembali ke standby)
     */
    void stop();

    /**
     * @brief Berhenti mendadak (Emergency freeze di posisi saat ini)
     */
    void emergency_freeze();

    /**
     * @brief Update loop non-blocking berbasis tick milidetik (panggil di main loop / FreeRTOS task)
     * @param current_tick_ms Nilai tick STM32 saat ini (misal HAL_GetTick() atau FreeRTOS xTaskGetTickCount())
     * @return bool True jika perhitungan IK semua kaki berhasil
     */
    bool update(uint32_t current_tick_ms);

    /**
     * @brief Atur parameter kecepatan dan geometri langkah secara dinamis
     */
    void set_step_config(float stride_length_mm, float step_height_mm, uint32_t period_ms);
    void set_velocity(float vx_ratio, float vy_ratio); // vx: [-1.0..1.0] maju/mundur, vy: [-1.0..1.0] geser

    // Getter data keluaran
    GaitState get_state() const { return state_; }
    bool is_walking() const { return state_ == GaitState::WALKING || state_ == GaitState::STARTING; }
    const HexapodJointAngles& get_joint_angles() const { return current_joint_angles_; }
    const Vector3D* get_foot_targets() const { return current_targets_; }
    LegPhase get_leg_phase(uint8_t leg_index) const;

    /**
     * @brief Ambil array 18 nilai posisi raw Dynamixel AX-12A siap kirim ke SyncWrite buffer
     * @param out_dxl_positions Array output berukuran 18 (uint16_t)
     */
    void get_sync_write_positions(uint16_t out_dxl_positions[TOTAL_SERVOS]) const;

private:
    /**
     * @brief Hitung lintasan koordinat 3D satu kaki pada fase tertentu
     * @param leg_index Indeks kaki (0..5)
     * @param normalized_phase Fase siklus [0.0 .. 1.0)
     * @return Vector3D Posisi target ujung kaki (lokal coxa)
     */
    Vector3D calculate_foot_trajectory(uint8_t leg_index, float normalized_phase) const;

    const KinematicsSolver& ik_solver_;
    GaitConfig config_;
    GaitState state_;

    uint32_t last_tick_ms_;
    uint32_t cycle_start_tick_ms_;
    float cycle_progress_; // 0.0f .. 1.0f

    float vx_scale_; // Kontrol kecepatan maju/mundur [-1.0 .. 1.0]
    float vy_scale_; // Kontrol geser lateral [-1.0 .. 1.0]

    Vector3D neutral_poses_[NUM_LEGS];
    Vector3D current_targets_[NUM_LEGS];
    HexapodJointAngles current_joint_angles_;

    // Tripod group:
    // Group 0: Kaki 0 (RF), 3 (LM), 4 (RB) -> Tripod A
    // Group 1: Kaki 1 (LF), 2 (RM), 5 (LB) -> Tripod B
    uint8_t leg_group_assignment_[NUM_LEGS];
};

} // namespace Movement

#endif // MOVEMENT_GAIT_H
