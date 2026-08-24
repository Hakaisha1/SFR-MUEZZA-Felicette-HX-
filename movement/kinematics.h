/**
 * @file kinematics.h
 * @brief Inverse and Forward Kinematics for 3-DOF Hexapod Robot Legs (STM32 C++)
 * @author Movement Engineer
 * @date 2026-08-22
 */

#ifndef MOVEMENT_KINEMATICS_H
#define MOVEMENT_KINEMATICS_H

#include <stdint.h>
#include <cmath>

namespace Movement {

// Konstanta Matematika
#ifndef PI_F
constexpr float PI_F = 3.14159265358979323846f;
#endif
constexpr float RAD_TO_DEG = 180.0f / PI_F;
constexpr float DEG_TO_RAD = PI_F / 180.0f;

// Total kaki dan servo
constexpr uint8_t NUM_LEGS = 6;
constexpr uint8_t JOINTS_PER_LEG = 3;
constexpr uint8_t TOTAL_SERVOS = NUM_LEGS * JOINTS_PER_LEG; // 18

/**
 * @brief Status hasil kalkulasi Inverse Kinematics
 */
enum class IKResult : uint8_t {
    SUCCESS = 0,                ///< Kalkulasi valid dan dapat dicapai
    ERR_OUT_OF_REACH,           ///< Target berada di luar jangkauan maksimum (L2 + L3)
    ERR_INSIDE_SINGULARITY,     ///< Target terlalu dekat ke pangkal / singularitas dalam
    ERR_ANGLE_LIMIT_EXCEEDED,   ///< Sudut melebihi batas fisik servo
    ERR_NAN_OR_INF              ///< Terjadi NaN / Division by zero
};

/**
 * @brief Representasi koordinat 3D Cartesian (dalam mm)
 */
struct Vector3D {
    float x;
    float y;
    float z;

    constexpr Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    float length_xy() const {
        return std::sqrt(x * x + y * y);
    }

    float length_3d() const {
        return std::sqrt(x * x + y * y + z * z);
    }
};

/**
 * @brief Dimensi mekanik segmen link kaki (dalam mm)
 */
struct LegDimensions {
    float coxa;   // L1 (mm) - segmen sendi horizontal coxa
    float femur;  // L2 (mm) - segmen paha
    float tibia;  // L3 (mm) - segmen betis

    constexpr LegDimensions(float c = 50.0f, float f = 70.0f, float t = 110.0f)
        : coxa(c), femur(f), tibia(t) {}
};

/**
 * @brief Batasan sudut fisik joint servo (dalam derajat)
 */
struct JointLimits {
    float min_coxa;
    float max_coxa;
    float min_femur;
    float max_femur;
    float min_tibia;
    float max_tibia;

    constexpr JointLimits(
        float min_c = -85.0f, float max_c = 85.0f,
        float min_f = -90.0f, float max_f = 90.0f,
        float min_t = -160.0f, float max_t = 20.0f
    ) : min_coxa(min_c), max_coxa(max_c),
        min_femur(min_f), max_femur(max_f),
        min_tibia(min_t), max_tibia(max_t) {}
};

/**
 * @brief Sudut 3 sendi pada 1 kaki (dalam derajat atau radian)
 */
struct LegJointAngles {
    float coxa;   // Sudut Coxa (Yaw / rotasi horizontal)
    float femur;  // Sudut Femur (Pitch / rotasi vertikal)
    float tibia;  // Sudut Tibia (Pitch / rotasi vertikal)

    constexpr LegJointAngles() : coxa(0.0f), femur(0.0f), tibia(0.0f) {}
    constexpr LegJointAngles(float c, float f, float t) : coxa(c), femur(f), tibia(t) {}

    // Konversi derajat ke Dynamixel AX-12A raw unit (0-1023, center = 512, 0.29 deg/unit)
    uint16_t coxa_to_ax12(uint16_t center = 512) const {
        return angle_to_dxl_ax12(coxa, center);
    }
    uint16_t femur_to_ax12(uint16_t center = 512) const {
        return angle_to_dxl_ax12(femur, center);
    }
    uint16_t tibia_to_ax12(uint16_t center = 512) const {
        return angle_to_dxl_ax12(tibia, center);
    }

private:
    static uint16_t angle_to_dxl_ax12(float deg, uint16_t center) {
        float raw = center + (deg / 0.29296875f);
        if (raw < 0.0f) raw = 0.0f;
        if (raw > 1023.0f) raw = 1023.0f;
        return static_cast<uint16_t>(raw + 0.5f);
    }
};

/**
 * @brief Konfigurasi orientasi & offset mounting kaki pada bodi robot
 */
struct LegMountConfig {
    Vector3D body_offset; // Posisi sendi coxa relatif terhadap pusat robot (x, y, z mm)
    float mount_angle_deg;// Orientasi pemasangan coxa relatif terhadap sumbu depan robot (derajat)
};

/**
 * @brief Sudut semua 18 joint servo untuk 6 kaki
 */
struct HexapodJointAngles {
    LegJointAngles legs[NUM_LEGS];
    IKResult status[NUM_LEGS];
    bool all_valid;
};

/**
 * @brief Kelas utama Kinematika Hexapod (IK & FK)
 */
class KinematicsSolver {
public:
    KinematicsSolver(const LegDimensions& dims = LegDimensions(), const JointLimits& limits = JointLimits());

    /**
     * @brief Set konfigurasi mounting kaki hexapod
     */
    void set_leg_mount(uint8_t leg_index, const Vector3D& offset, float mount_angle_deg);

    /**
     * @brief Hitung Inverse Kinematics untuk 1 kaki (dalam koordinat lokal coxa)
     * @param target Koordinat target ujung kaki relatif terhadap sendi Coxa (x, y, z dalam mm)
     * @param out_angles Output sudut ketiga servo (dalam derajat)
     * @return IKResult Status validitas kalkulasi
     */
    IKResult compute_leg_ik_local(const Vector3D& target, LegJointAngles& out_angles) const;

    /**
     * @brief Hitung Inverse Kinematics untuk 1 kaki (dalam koordinat pusat bodi robot)
     * @param leg_index Indeks kaki (0..5)
     * @param target_body Koordinat target relatif terhadap pusat bodi hexapod
     * @param out_angles Output sudut ketiga servo (dalam derajat)
     * @return IKResult Status validitas kalkulasi
     */
    IKResult compute_leg_ik_body(uint8_t leg_index, const Vector3D& target_body, LegJointAngles& out_angles) const;

    /**
     * @brief Hitung Inverse Kinematics untuk seluruh 6 kaki (18 servo) secara serentak
     * @param targets_local Array 6 koordinat target ujung kaki (lokal coxa)
     * @param out_all Output struct sudut 18 servo
     * @return bool True jika semua 6 kaki valid
     */
    bool compute_all_legs_ik(const Vector3D targets_local[NUM_LEGS], HexapodJointAngles& out_all) const;

    /**
     * @brief Forward Kinematics (FK): Hitung posisi ujung kaki dari sudut 3 sendi (dalam mm)
     * @param angles Sudut ketiga sendi (derajat)
     * @param out_pos Output koordinat ujung kaki (lokal coxa)
     */
    void compute_leg_fk(const LegJointAngles& angles, Vector3D& out_pos) const;

    /**
     * @brief Fungsi validasi rentang gerak dan deteksi singularitas
     * @param target Posisi target lokal coxa
     * @return IKResult
     */
    IKResult check_reachability(const Vector3D& target) const;

    /**
     * @brief Validasi lintasan/grid workspace dan cetak plot ASCII visual ke string buffer
     * @param out_buffer Buffer tujuan untuk teks report / plot ASCII
     * @param buffer_size Ukuran buffer
     * @param z_height Ketinggian Z uji (misal -90.0 mm)
     */
    void generate_workspace_ascii_plot(char* out_buffer, uint32_t buffer_size, float z_height = -90.0f) const;

    // Getter dimensi dan batasan
    const LegDimensions& get_dimensions() const { return dims_; }
    const JointLimits& get_limits() const { return limits_; }

private:
    LegDimensions dims_;
    JointLimits limits_;
    LegMountConfig mounts_[NUM_LEGS];
};

} // namespace Movement

#endif // MOVEMENT_KINEMATICS_H
