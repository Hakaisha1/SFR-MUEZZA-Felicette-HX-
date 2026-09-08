/**
 * @file gripper.h
 * @brief Kontrol Gripper 2-DOF (Pitch + Yaw) via Dynamixel AX-12A
 * @author Movement Engineer
 * @date 2026-09-08
 *
 * Gripper menggunakan 2 servo Dynamixel:
 *   - Pitch (ID 0): Menggerakkan gripper naik/turun
 *   - Yaw   (ID 33): Menggerakkan gripper buka/tutup (jepit)
 *
 * Referensi ID dari: dynamixel_id.md
 */

#ifndef MOVEMENT_GRIPPER_H
#define MOVEMENT_GRIPPER_H

#include <stdint.h>

namespace Movement {

// ============================================================
// Konfigurasi ID Servo Gripper
// ============================================================
constexpr uint8_t GRIPPER_PITCH_ID = 0;   // Servo pitch (naik/turun)
constexpr uint8_t GRIPPER_YAW_ID   = 33;  // Servo yaw (buka/tutup)

// Total servo gripper (terpisah dari 18 servo kaki)
constexpr uint8_t GRIPPER_NUM_SERVOS = 2;

// ============================================================
// Batas Sudut Gripper (derajat, relatif terhadap center=150°)
// Sesuaikan setelah kalibrasi mekanis!
// ============================================================
constexpr float GRIPPER_PITCH_MIN_DEG = -60.0f;   // Posisi paling bawah
constexpr float GRIPPER_PITCH_MAX_DEG =  60.0f;   // Posisi paling atas
constexpr float GRIPPER_YAW_MIN_DEG   =  0.0f;    // Buka penuh
constexpr float GRIPPER_YAW_MAX_DEG   =  90.0f;   // Tutup penuh (jepit)

// Posisi default saat standby
constexpr float GRIPPER_PITCH_STANDBY_DEG = 0.0f;  // Horizontal / netral
constexpr float GRIPPER_YAW_STANDBY_DEG   = 0.0f;  // Buka

// ============================================================
// State Gripper
// ============================================================

/**
 * @brief State operasional gripper
 */
enum class GripperState : uint8_t {
    IDLE = 0,       ///< Gripper standby, tidak aktif
    OPENING,        ///< Gripper sedang membuka
    CLOSING,        ///< Gripper sedang menutup/menjepit
    GRIPPING,       ///< Gripper sudah menjepit objek (holding)
    RELEASING,      ///< Gripper sedang melepas objek
    ERROR           ///< Gripper error (torque overload, dll)
};

/**
 * @brief Perintah manipulator dari Integration (via CMD_MANIPULATOR)
 */
enum class GripperCommand : uint8_t {
    NONE = 0,
    GRIP,           ///< Jepit objek (tutup yaw + turunkan pitch)
    RELEASE,        ///< Lepas objek (buka yaw + naikkan pitch)
    SET_ANGLE,      ///< Set sudut pitch & yaw secara manual
    STANDBY         ///< Kembali ke posisi netral
};

/**
 * @brief Konfigurasi sudut target gripper
 */
struct GripperTarget {
    float pitch_deg;   // Sudut pitch target (derajat)
    float yaw_deg;     // Sudut yaw target (derajat)

    constexpr GripperTarget()
        : pitch_deg(GRIPPER_PITCH_STANDBY_DEG),
          yaw_deg(GRIPPER_YAW_STANDBY_DEG) {}

    constexpr GripperTarget(float p, float y)
        : pitch_deg(p), yaw_deg(y) {}
};

/**
 * @brief Posisi raw Dynamixel untuk kedua servo gripper
 */
struct GripperDxlPositions {
    uint16_t pitch_raw;  // Posisi raw AX-12A untuk servo pitch (0–1023)
    uint16_t yaw_raw;    // Posisi raw AX-12A untuk servo yaw (0–1023)
};

/**
 * @brief Kelas kontrol Gripper 2-DOF
 *
 * Menyediakan:
 *   - Konversi sudut derajat → raw AX-12A
 *   - State machine open/close/grip/release
 *   - Posisi raw siap kirim ke Dynamixel SyncWrite
 */
class GripperController {
public:
    GripperController();

    /**
     * @brief Inisialisasi gripper ke posisi standby
     */
    void init();

    /**
     * @brief Eksekusi perintah gripper
     * @param cmd Perintah dari Integration (GRIP, RELEASE, SET_ANGLE, STANDBY)
     * @param target Sudut target (hanya dipakai untuk SET_ANGLE)
     */
    void execute(GripperCommand cmd, const GripperTarget& target = GripperTarget());

    /**
     * @brief Update loop gripper (panggil periodik dari main loop)
     *
     * Menggerakkan servo secara gradual menuju posisi target
     * untuk menghindari gerakan terlalu kasar.
     *
     * @param current_tick_ms Tick STM32 saat ini
     * @return bool True jika posisi sudah mencapai target
     */
    bool update(uint32_t current_tick_ms);

    // ---- Getter ----
    GripperState get_state() const { return state_; }
    bool is_gripping() const { return state_ == GripperState::GRIPPING; }
    bool is_idle() const { return state_ == GripperState::IDLE; }

    /**
     * @brief Ambil posisi raw Dynamixel untuk SyncWrite
     */
    GripperDxlPositions get_dxl_positions() const;

    /**
     * @brief Ambil sudut saat ini (derajat)
     */
    float get_current_pitch_deg() const { return current_pitch_deg_; }
    float get_current_yaw_deg() const { return current_yaw_deg_; }

private:
    /**
     * @brief Konversi sudut derajat ke posisi raw AX-12A (0–1023)
     * @param deg Sudut dalam derajat
     * @param center Posisi center raw (default 512 = 150°)
     * @return uint16_t Posisi raw
     */
    static uint16_t deg_to_ax12_raw(float deg, uint16_t center = 512);

    /**
     * @brief Clamp sudut ke batas fisik
     */
    static float clamp_pitch(float deg);
    static float clamp_yaw(float deg);

    GripperState state_;

    // Posisi saat ini (derajat)
    float current_pitch_deg_;
    float current_yaw_deg_;

    // Posisi target (derajat)
    float target_pitch_deg_;
    float target_yaw_deg_;

    // Timing untuk gerakan gradual
    uint32_t last_update_tick_ms_;
    static constexpr float MOVE_SPEED_DEG_PER_SEC = 120.0f; // Kecepatan gerak (deg/s)
};

} // namespace Movement

#endif // MOVEMENT_GRIPPER_H
