/**
 * @file test_kinematics.cpp
 * @brief Program uji validasi matematis & plot visual workspace Inverse Kinematics
 */

#include "kinematics.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace Movement;

void test_single_leg_and_fk_validation() {
    std::cout << "========================================================\n";
    std::cout << "TEST 1: IK & FK Consistency Check (Roundtrip Accuracy)\n";
    std::cout << "========================================================\n";

    KinematicsSolver solver;
    
    // Uji beberapa titik koordinat realistis kaki hexapod
    std::vector<Vector3D> test_targets = {
        Vector3D(120.0f,   0.0f, -80.0f),  // Netral / Standby
        Vector3D(140.0f,  40.0f, -90.0f),  // Langkah depan kanan
        Vector3D(100.0f, -40.0f, -70.0f),  // Langkah belakang kiri
        Vector3D(160.0f,   0.0f, -60.0f),  // Extensi jauh
        Vector3D( 90.0f,   0.0f, -110.0f)  // Jongkok / rendah
    };

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Target (X, Y, Z) mm       | IK Sudut (Coxa, Femur, Tibia) deg | FK Hasil (X, Y, Z) mm       | Error (mm)\n";
    std::cout << "--------------------------+-----------------------------------+-----------------------------+-----------\n";

    for (const auto& target : test_targets) {
        LegJointAngles angles;
        IKResult res = solver.compute_leg_ik_local(target, angles);

        if (res == IKResult::SUCCESS) {
            Vector3D fk_pos;
            solver.compute_leg_fk(angles, fk_pos);

            float err_x = fk_pos.x - target.x;
            float err_y = fk_pos.y - target.y;
            float err_z = fk_pos.z - target.z;
            float total_err = std::sqrt(err_x * err_x + err_y * err_y + err_z * err_z);

            std::cout << "(" << std::setw(6) << target.x << ", " << std::setw(6) << target.y << ", " << std::setw(6) << target.z << ") | "
                      << "(" << std::setw(6) << angles.coxa << ", " << std::setw(6) << angles.femur << ", " << std::setw(6) << angles.tibia << ") | "
                      << "(" << std::setw(6) << fk_pos.x << ", " << std::setw(6) << fk_pos.y << ", " << std::setw(6) << fk_pos.z << ") | "
                      << std::setw(7) << total_err << " mm (OK)\n";
        } else {
            std::cout << "(" << target.x << ", " << target.y << ", " << target.z << ") | GAGAL / Out of range (" << (int)res << ")\n";
        }
    }
    std::cout << "\n";
}

void test_six_legs_batch() {
    std::cout << "========================================================\n";
    std::cout << "TEST 2: Perulangan 6 Kaki (18 Servo Total) Batch IK\n";
    std::cout << "========================================================\n";

    KinematicsSolver solver;
    Vector3D standing_pose[6] = {
        Vector3D(130.0f,  0.0f, -85.0f), // Leg 0 (RF)
        Vector3D(130.0f,  0.0f, -85.0f), // Leg 1 (LF)
        Vector3D(130.0f,  0.0f, -85.0f), // Leg 2 (RM)
        Vector3D(130.0f,  0.0f, -85.0f), // Leg 3 (LM)
        Vector3D(130.0f,  0.0f, -85.0f), // Leg 4 (RB)
        Vector3D(130.0f,  0.0f, -85.0f)  // Leg 5 (LB)
    };

    HexapodJointAngles all_joints;
    bool success = solver.compute_all_legs_ik(standing_pose, all_joints);

    std::cout << "Batch Status: " << (success ? "SEMUA 6 KAKI VALID (SUCCESS)" : "ADA KAKI GAGAL") << "\n";
    for (uint8_t i = 0; i < NUM_LEGS; ++i) {
        const auto& leg = all_joints.legs[i];
        std::cout << "Kaki " << (int)i << " -> Coxa: " << std::setw(6) << leg.coxa 
                  << " deg [AX12:" << std::setw(4) << leg.coxa_to_ax12() << "]"
                  << " | Femur: " << std::setw(6) << leg.femur 
                  << " deg [AX12:" << std::setw(4) << leg.femur_to_ax12() << "]"
                  << " | Tibia: " << std::setw(6) << leg.tibia 
                  << " deg [AX12:" << std::setw(4) << leg.tibia_to_ax12() << "]\n";
    }
    std::cout << "\n";
}

void test_singularities_and_bounds() {
    std::cout << "========================================================\n";
    std::cout << "TEST 3: Singularitas & Boundary Protection Checks\n";
    std::cout << "========================================================\n";

    KinematicsSolver solver;
    LegJointAngles dummy;

    // Titik terlalu jauh (> 50 + 70 + 110 = 230 mm)
    Vector3D far_pt(250.0f, 0.0f, 0.0f);
    IKResult res_far = solver.compute_leg_ik_local(far_pt, dummy);
    std::cout << "Target (250, 0, 0) Terlalu Jauh    -> Hasil: " 
              << (res_far == IKResult::ERR_OUT_OF_REACH ? "PASS (ERR_OUT_OF_REACH)" : "FAIL") << "\n";

    // Titik terlalu dekat (singularitas dalam)
    Vector3D close_pt(10.0f, 0.0f, 0.0f);
    IKResult res_close = solver.compute_leg_ik_local(close_pt, dummy);
    std::cout << "Target (10, 0, 0) Terlalu Dekat   -> Hasil: " 
              << (res_close == IKResult::ERR_INSIDE_SINGULARITY ? "PASS (ERR_INSIDE_SINGULARITY)" : "FAIL") << "\n";

    // Titik NaN / Inf
    Vector3D nan_pt(NAN, 0.0f, -80.0f);
    IKResult res_nan = solver.compute_leg_ik_local(nan_pt, dummy);
    std::cout << "Target NaN                         -> Hasil: " 
              << (res_nan == IKResult::ERR_NAN_OR_INF ? "PASS (ERR_NAN_OR_INF)" : "FAIL") << "\n";

    std::cout << "\n";
}

void test_ascii_workspace_plot() {
    std::cout << "========================================================\n";
    std::cout << "TEST 4: Visual Workspace 2D Slice ASCII Plot\n";
    std::cout << "========================================================\n";

    KinematicsSolver solver;
    char buffer[2048];
    solver.generate_workspace_ascii_plot(buffer, sizeof(buffer), -85.0f);
    std::cout << buffer << "\n";
}

int main() {
    test_single_leg_and_fk_validation();
    test_six_legs_batch();
    test_singularities_and_bounds();
    test_ascii_workspace_plot();
    return 0;
}
