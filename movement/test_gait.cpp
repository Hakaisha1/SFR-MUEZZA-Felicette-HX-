/**
 * @file test_gait.cpp
 * @brief Program uji validasi siklus Tripod Gait Generator & IK 18 Servo
 */

#include "gait.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace Movement;

int main() {
    std::cout << "====================================================================\n";
    std::cout << "TEST TRIPOD GAIT GENERATOR (Full Cycle 800ms, Step 50ms interval)\n";
    std::cout << "====================================================================\n";

    KinematicsSolver ik_solver;
    GaitConfig config;
    config.stride_length = 40.0f;     // Langkah 40 mm
    config.step_height = 25.0f;       // Angkat kaki 25 mm
    config.cycle_period_ms = 800;     // 1 siklus 800 ms
    config.neutral_reach_x = 130.0f;  // X netral = 130 mm
    config.neutral_height_z = -85.0f; // Z netral = -85 mm

    TripodGaitGenerator gait(ik_solver, config);
    gait.start();

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Time(ms) | Phase | Tripod A (RF) Pos (X, Z) | Phase | Tripod B (LF) Pos (X, Z) | Phase | All IK Valid?\n";
    std::cout << "---------+-------+--------------------------+-------+--------------------------+-------+---------------\n";

    bool all_steps_valid = true;
    for (uint32_t t = 0; t <= 800; t += 50) {
        bool ik_ok = gait.update(t);
        if (!ik_ok) all_steps_valid = false;

        const Vector3D* targets = gait.get_foot_targets();
        LegPhase phase_rf = gait.get_leg_phase(0); // RF (Grup 0)
        LegPhase phase_lf = gait.get_leg_phase(1); // LF (Grup 1)

        std::cout << std::setw(6) << t << " ms | "
                  << (phase_rf == LegPhase::SWING ? "SWING" : "STNCE") << " | "
                  << "X=" << std::setw(5) << targets[0].x << " Z=" << std::setw(5) << targets[0].z << " mm | "
                  << (phase_lf == LegPhase::SWING ? "SWING" : "STNCE") << " | "
                  << "X=" << std::setw(5) << targets[1].x << " Z=" << std::setw(5) << targets[1].z << " mm | "
                  << (ik_ok ? " [PASS 18 Servo OK]" : " [FAIL IK ERROR]") << "\n";
    }

    std::cout << "\n====================================================================\n";
    std::cout << "SAMPLE DYNAMIXEL AX-12A PACKET POSITIONS (at t = 200 ms - Mid-Swing A)\n";
    std::cout << "====================================================================\n";
    
    gait.update(200);
    uint16_t dxl_servos[TOTAL_SERVOS];
    gait.get_sync_write_positions(dxl_servos);

    for (uint8_t leg = 0; leg < NUM_LEGS; ++leg) {
        const char* leg_names[6] = {"RF (0)", "LF (1)", "RM (2)", "LM (3)", "RB (4)", "LB (5)"};
        uint8_t base_idx = leg * JOINTS_PER_LEG;
        std::cout << "Kaki " << leg_names[leg] 
                  << " -> Coxa: " << std::setw(4) << dxl_servos[base_idx]
                  << " | Femur: " << std::setw(4) << dxl_servos[base_idx + 1]
                  << " | Tibia: " << std::setw(4) << dxl_servos[base_idx + 2]
                  << " (" << (gait.get_leg_phase(leg) == LegPhase::SWING ? "SWING" : "STANCE") << ")\n";
    }

    std::cout << "\nStatus Keseluruhan: " << (all_steps_valid ? "SUKSES SEMPURNA (100% Valid)" : "GAGAL") << "\n";

    return 0;
}
