/**
 * @file kinematics.cpp
 * @brief Implementasi Inverse Kinematics dan Forward Kinematics 3-DOF Hexapod untuk STM32
 * @author Movement Engineer
 * @date 2026-08-22
 */

#include "kinematics.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace Movement {

KinematicsSolver::KinematicsSolver(const LegDimensions& dims, const JointLimits& limits)
    : dims_(dims), limits_(limits) {
    
    // Konfigurasi default mounting 6 kaki (posisi coxa & sudut hadap)
    // 0: Right Front (RF), 1: Left Front (LF)
    // 2: Right Middle (RM), 3: Left Middle (LM)
    // 4: Right Back (RB),  5: Left Back (LB)
    const float default_mount_angles[NUM_LEGS] = {
        -45.0f,  // 0: RF
         45.0f,  // 1: LF
        -90.0f,  // 2: RM
         90.0f,  // 3: LM
       -135.0f,  // 4: RB
        135.0f   // 5: LB
    };

    const Vector3D default_offsets[NUM_LEGS] = {
        Vector3D( 120.0f, -80.0f, 0.0f), // 0: RF
        Vector3D( 120.0f,  80.0f, 0.0f), // 1: LF
        Vector3D(   0.0f, -100.0f, 0.0f), // 2: RM
        Vector3D(   0.0f,  100.0f, 0.0f), // 3: LM
        Vector3D(-120.0f, -80.0f, 0.0f), // 4: RB
        Vector3D(-120.0f,  80.0f, 0.0f)  // 5: LB
    };

    for (uint8_t i = 0; i < NUM_LEGS; ++i) {
        mounts_[i].body_offset = default_offsets[i];
        mounts_[i].mount_angle_deg = default_mount_angles[i];
    }
}

void KinematicsSolver::set_leg_mount(uint8_t leg_index, const Vector3D& offset, float mount_angle_deg) {
    if (leg_index < NUM_LEGS) {
        mounts_[leg_index].body_offset = offset;
        mounts_[leg_index].mount_angle_deg = mount_angle_deg;
    }
}

IKResult KinematicsSolver::check_reachability(const Vector3D& target) const {
    if (std::isnan(target.x) || std::isnan(target.y) || std::isnan(target.z) ||
        std::isinf(target.x) || std::isinf(target.y) || std::isinf(target.z)) {
        return IKResult::ERR_NAN_OR_INF;
    }

    float r_xy = std::sqrt(target.x * target.x + target.y * target.y);
    float r_femur = r_xy - dims_.coxa;
    float d_sq = (r_femur * r_femur) + (target.z * target.z);
    float d = std::sqrt(d_sq);

    // Singularitas: Target berada di dalam radius coxa atau di bawah batas mekanik link
    if (r_femur <= 0.0f) {
        return IKResult::ERR_INSIDE_SINGULARITY;
    }

    // Jangkauan maksimum (kaki lurus penuh)
    float max_reach = dims_.femur + dims_.tibia;
    if (d > max_reach) {
        return IKResult::ERR_OUT_OF_REACH;
    }

    // Jangkauan minimum segitiga femur-tibia
    float min_reach = std::fabs(dims_.femur - dims_.tibia);
    if (d <= min_reach) {
        return IKResult::ERR_INSIDE_SINGULARITY;
    }

    return IKResult::SUCCESS;
}

IKResult KinematicsSolver::compute_leg_ik_local(const Vector3D& target, LegJointAngles& out_angles) const {
    // 1. Cek validitas titik target & jangkauan
    IKResult reach_status = check_reachability(target);
    if (reach_status != IKResult::SUCCESS) {
        return reach_status;
    }

    // 2. Hitung Sudut Coxa (theta1: Yaw)
    // Sumbu X: arah maju/panjang kaki, Sumbu Y: arah samping
    float theta1_rad = std::atan2(target.y, target.x);
    float theta1_deg = theta1_rad * RAD_TO_DEG;

    // 3. Proyeksi ke bidang 2D Femur-Tibia
    float r_xy = std::sqrt(target.x * target.x + target.y * target.y);
    float r_femur = r_xy - dims_.coxa;
    float d_sq = (r_femur * r_femur) + (target.z * target.z);
    float d = std::sqrt(d_sq);

    // Mencegah pembagian dengan nol
    if (d < 1e-4f) {
        return IKResult::ERR_INSIDE_SINGULARITY;
    }

    // 4. Hitung Sudut Tibia (theta3: Pitch) via Law of Cosines
    // Diberikan: d^2 = L2^2 + L3^2 - 2*L2*L3*cos(pi - theta3)
    // cos(gamma) = (L2^2 + L3^2 - d^2) / (2 * L2 * L3)
    float cos_gamma = (dims_.femur * dims_.femur + dims_.tibia * dims_.tibia - d_sq) / 
                      (2.0f * dims_.femur * dims_.tibia);
    
    // Clamp nilai cosine ke rentang [-1.0, 1.0] untuk menghindari domain error acosf
    cos_gamma = std::fmax(-1.0f, std::fmin(1.0f, cos_gamma));
    float gamma_rad = std::acos(cos_gamma); // Sudut interior antara femur dan tibia

    // Sudut servo tibia relatif terhadap ekstensi garis femur (defleksi)
    // 0 deg = lurus dengan femur, negatif = menekuk ke bawah
    float theta3_deg = (gamma_rad * RAD_TO_DEG) - 180.0f;

    // 5. Hitung Sudut Femur (theta2: Pitch)
    // alpha = sudut garis d terhadap bidang horizontal
    // beta = sudut interior antara femur dan garis d
    float alpha_rad = std::atan2(target.z, r_femur); // z negatif ke bawah
    
    float cos_beta = (dims_.femur * dims_.femur + d_sq - dims_.tibia * dims_.tibia) / 
                     (2.0f * dims_.femur * d);
    cos_beta = std::fmax(-1.0f, std::fmin(1.0f, cos_beta));
    float beta_rad = std::acos(cos_beta);

    float theta2_rad = alpha_rad + beta_rad;
    float theta2_deg = theta2_rad * RAD_TO_DEG;

    // 6. Cek Batasan Sudut Fisik Servo
    if (theta1_deg < limits_.min_coxa  || theta1_deg > limits_.max_coxa ||
        theta2_deg < limits_.min_femur || theta2_deg > limits_.max_femur ||
        theta3_deg < limits_.min_tibia || theta3_deg > limits_.max_tibia) {
        
        // Simpan sudut meski batas terlampaui agar caller dapat menganalisis
        out_angles = LegJointAngles(theta1_deg, theta2_deg, theta3_deg);
        return IKResult::ERR_ANGLE_LIMIT_EXCEEDED;
    }

    out_angles = LegJointAngles(theta1_deg, theta2_deg, theta3_deg);
    return IKResult::SUCCESS;
}

IKResult KinematicsSolver::compute_leg_ik_body(uint8_t leg_index, const Vector3D& target_body, LegJointAngles& out_angles) const {
    if (leg_index >= NUM_LEGS) {
        return IKResult::ERR_NAN_OR_INF;
    }

    const LegMountConfig& mount = mounts_[leg_index];

    // 1. Translasi titik target relatif terhadap pangkal coxa
    float dx = target_body.x - mount.body_offset.x;
    float dy = target_body.y - mount.body_offset.y;
    float dz = target_body.z - mount.body_offset.z;

    // 2. Rotasi sumbu sesuai orientasi pemasangan coxa pada bodi robot
    float mount_rad = -mount.mount_angle_deg * DEG_TO_RAD; // rotasi balik
    float cos_m = std::cos(mount_rad);
    float sin_m = std::sin(mount_rad);

    float local_x = dx * cos_m - dy * sin_m;
    float local_y = dx * sin_m + dy * cos_m;
    float local_z = dz;

    Vector3D local_target(local_x, local_y, local_z);
    return compute_leg_ik_local(local_target, out_angles);
}

bool KinematicsSolver::compute_all_legs_ik(const Vector3D targets_local[NUM_LEGS], HexapodJointAngles& out_all) const {
    out_all.all_valid = true;
    for (uint8_t i = 0; i < NUM_LEGS; ++i) {
        out_all.status[i] = compute_leg_ik_local(targets_local[i], out_all.legs[i]);
        if (out_all.status[i] != IKResult::SUCCESS) {
            out_all.all_valid = false;
        }
    }
    return out_all.all_valid;
}

void KinematicsSolver::compute_leg_fk(const LegJointAngles& angles, Vector3D& out_pos) const {
    float theta1_rad = angles.coxa * DEG_TO_RAD;
    float theta2_rad = angles.femur * DEG_TO_RAD;
    // theta3 defleksi relatif femur (gamma = theta3 + 180 deg)
    float theta23_rad = (angles.femur + angles.tibia) * DEG_TO_RAD;

    // Proyeksi jarak horizontal pada bidang kaki
    float r_femur_tibia = dims_.femur * std::cos(theta2_rad) + dims_.tibia * std::cos(theta23_rad);
    float r_total = dims_.coxa + r_femur_tibia;

    out_pos.x = r_total * std::cos(theta1_rad);
    out_pos.y = r_total * std::sin(theta1_rad);
    out_pos.z = dims_.femur * std::sin(theta2_rad) + dims_.tibia * std::sin(theta23_rad);
}

void KinematicsSolver::generate_workspace_ascii_plot(char* out_buffer, uint32_t buffer_size, float z_height) const {
    if (!out_buffer || buffer_size < 512) return;

    int written = std::snprintf(out_buffer, buffer_size,
        "=== HEXAPOD LEG WORKSPACE 2D SLICE (Z = %.1f mm) ===\n"
        "Dims: Coxa=%.0f, Femur=%.0f, Tibia=%.0f mm\n"
        "Legend: '.' = Reachable/Valid, 'X' = Out of Reach / Limit, 'O' = Center\n"
        "     X -> (60 mm to 220 mm), Y -> (-100 mm to +100 mm)\n\n",
        z_height, dims_.coxa, dims_.femur, dims_.tibia);

    if (written < 0 || static_cast<uint32_t>(written) >= buffer_size) return;
    char* ptr = out_buffer + written;
    uint32_t remaining = buffer_size - written;

    // Grid sweep X dan Y
    for (float y = 100.0f; y >= -100.0f; y -= 20.0f) {
        char line[64];
        int pos = std::snprintf(line, sizeof(line), "Y%+4.0f | ", y);
        
        for (float x = 60.0f; x <= 220.0f; x += 10.0f) {
            Vector3D test_pt(x, y, z_height);
            LegJointAngles angles;
            IKResult res = compute_leg_ik_local(test_pt, angles);

            char marker = ' ';
            if (res == IKResult::SUCCESS) {
                marker = '.';
            } else if (res == IKResult::ERR_ANGLE_LIMIT_EXCEEDED) {
                marker = 'L'; // Angle Limit
            } else {
                marker = 'X'; // Out of reach / singularity
            }
            if (pos < static_cast<int>(sizeof(line) - 2)) {
                line[pos++] = marker;
            }
        }
        line[pos++] = '\n';
        line[pos] = '\0';

        if (remaining > static_cast<uint32_t>(pos)) {
            std::strncat(ptr, line, remaining);
            ptr += pos;
            remaining -= pos;
        }
    }
}

} // namespace Movement
