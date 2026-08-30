#ifndef DYNAMIXEL_CONFIG_H
#define DYNAMIXEL_CONFIG_H

#include <stdint.h>

/**
 * @brief Mapping ID Dynamixel untuk 18 Servo Kaki (Hexapod) dan 1 Gripper.
 * Berdasarkan referensi dynamixel_id.md.
 */

// Indeks posisi kaki untuk array
typedef enum {
    LEG_KIRI_A = 0,
    LEG_KIRI_B,
    LEG_KIRI_C,
    LEG_KANAN_A,
    LEG_KANAN_B,
    LEG_KANAN_C,
    NUM_LEGS = 6
} LegIndex_t;

// Struct penyimpan ID untuk 1 kaki (3 DOF)
typedef struct {
    uint8_t coxa;
    uint8_t femur;
    uint8_t tibia;
} LegDynamixelIDs_t;

// Tabel Konstan ID Servo
static const LegDynamixelIDs_t DXL_ID_MAP[NUM_LEGS] = {
    // Kiri A
    { .coxa = 4,  .femur = 10, .tibia = 16 },
    // Kiri B (Tibia 18: ada catatan kabel harus direct)
    { .coxa = 6,  .femur = 12, .tibia = 18 },
    // Kiri C
    { .coxa = 3,  .femur = 9,  .tibia = 15 },
    // Kanan A
    { .coxa = 1,  .femur = 7,  .tibia = 13 },
    // Kanan B (Coxa 2 masih dibersihkan)
    { .coxa = 2,  .femur = 8,  .tibia = 14 },
    // Kanan C (Urutan memutar karena kabel)
    { .coxa = 17, .femur = 11, .tibia = 5  }
};

// ID Komponen Lain
#define DXL_ID_GRIPPER 0

#endif // DYNAMIXEL_CONFIG_H
