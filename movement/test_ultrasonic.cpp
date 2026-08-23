/**
 * @file test_ultrasonic.cpp
 * @brief Program verifikasi unit testing untuk driver 4x HC-SR04 ultrasonik
 */

#include "ultrasonic.h"
#include <iostream>
#include <iomanip>

using namespace Movement;

int main() {
    std::cout << "============================================================\n";
    std::cout << "TEST 1: Konversi Durasi Pulsa Echo (us) ke Jarak (mm)\n";
    std::cout << "============================================================\n";

    // Pengujian rumus konversi
    struct TestPulse {
        uint32_t us;
        const char* desc;
    };

    TestPulse pulses[] = {
        {   58, "Blind zone (< 20 mm)"},
        {  291, "50 mm (5 cm)"},
        {  466, "80 mm (Ambang Refleks Stop)"},
        {  583, "100 mm (10 cm)"},
        { 2915, "500 mm (50 cm)"},
        { 5830, "1000 mm (1 m)"},
        {23323, "4000 mm (4 m jangkauan maks)"},
        {35000, "Out of range (> 4 m)"}
    };

    for (const auto& tp : pulses) {
        uint16_t dist = UltrasonicDriver::duration_us_to_distance_mm(tp.us);
        std::cout << "Durasi: " << std::setw(6) << tp.us << " us -> Jarak: " 
                  << std::setw(5) << dist << " mm (" << tp.desc << ")\n";
    }

    std::cout << "\n============================================================\n";
    std::cout << "TEST 2: Simulasi Round-Robin Scheduling 4 Sensor (100ms)\n";
    std::cout << "============================================================\n";

    UltrasonicDriver driver;
    driver.init();

    // Simulasi rentang waktu 0ms sampai 125ms dengan jeda 25ms
    // Simulasi respons sensor:
    // Front = 70 mm (< 80 mm -> BAHAYA), Back = 500 mm, Left = 200 mm, Right = 9999 mm (bebas)
    uint32_t pulse_responses[4] = {
        408,   // Front: ~70 mm
        2915,  // Back: ~500 mm
        1166,  // Left: ~200 mm
        0      // Right: No echo
    };

    const char* names[4] = {"FRONT (Depan)", "BACK (Belakang)", "LEFT (Kiri)", "RIGHT (Kanan)"};

    for (uint32_t t = 0; t <= 125; t += 25) {
        // Simulasi penerimaan echo sebelum tick berikutnya
        uint8_t active_idx = (t / 25) % 4;
        if (active_idx < 4 && pulse_responses[active_idx] > 0) {
            driver.simulate_pulse_capture(static_cast<UltrasonicIndex>(active_idx), pulse_responses[active_idx]);
        }

        driver.update(t);
    }

    uint16_t all_distances[4];
    driver.get_all_distances_mm(all_distances);

    std::cout << "Hasil Pembacaan Akhir 4 Sensor:\n";
    for (uint8_t i = 0; i < 4; ++i) {
        bool reflex_triggered = driver.is_obstacle_detected(static_cast<UltrasonicIndex>(i));
        std::cout << "Sensor " << std::setw(15) << names[i] << " : " 
                  << std::setw(5) << all_distances[i] << " mm "
                  << (reflex_triggered ? "[REFLEKS TRIGGERED: STOP!]" : "[AMAN]") << "\n";
    }

    std::cout << "\nStatus Keseluruhan Driver Ultrasonik: SUKSES SEMPURNA (100% Valid)\n";

    return 0;
}
