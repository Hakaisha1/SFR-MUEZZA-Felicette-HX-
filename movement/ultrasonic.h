/**
 * @file ultrasonic.h
 * @brief Driver 4x Sensor Ultrasonik HC-SR04 (Timer Input-Capture) untuk STM32
 * @author Movement Engineer
 * @date 2026-08-22
 */

#ifndef MOVEMENT_ULTRASONIC_H
#define MOVEMENT_ULTRASONIC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(STM32F405xx) || defined(STM32F401xC) || defined(USE_HAL_DRIVER) || defined(__arm__)
#include "stm32f4xx_hal.h"
#define ULTRASONIC_STM32_HAL 1
#else
// Mock definitions untuk kompilasi testing di PC/Host
typedef void* GPIO_TypeDef;
typedef void* TIM_HandleTypeDef;
#define ULTRASONIC_STM32_HAL 0
#endif

namespace Movement {

// Jumlah sensor ultrasonik pada robot hexapod
constexpr uint8_t NUM_ULTRASONIC_SENSORS = 4;

// Batasan jarak fisik sensor HC-SR04 (dalam milimeter)
constexpr uint16_t MIN_VALID_DISTANCE_MM = 20;     // 2 cm (di bawah ini blind spot)
constexpr uint16_t MAX_VALID_DISTANCE_MM = 4000;   // 400 cm (4 meter jangkauan maks)
constexpr uint16_t OUT_OF_RANGE_DISTANCE_MM = 9999;// Indikator tidak ada pantulan
constexpr uint16_t REFLEX_THRESHOLD_MM = 80;       // 8 cm ambang batas refleks darurat

// Jeda antar trigger sensor (mencegah crosstalk/pantulan silang)
constexpr uint32_t INTER_SENSOR_INTERVAL_MS = 25;  // 25ms per sensor (100ms full 4-sensor cycle)
constexpr uint32_t TRIG_PULSE_DURATION_US = 10;    // 10us pulsa trigger

/**
 * @brief Indeks posisi 4 sensor ultrasonik
 */
enum UltrasonicIndex : uint8_t {
    US_FRONT = 0,    ///< Sensor Depan
    US_BACK  = 1,    ///< Sensor Belakang
    US_LEFT  = 2,    ///< Sensor Kiri
    US_RIGHT = 3     ///< Sensor Kanan
};

/**
 * @brief Konfigurasi Pin Hardware untuk satu sensor HC-SR04
 */
struct UltrasonicSensorConfig {
    GPIO_TypeDef* trig_port;     ///< GPIO Port untuk Pin TRIG (Output Push-Pull)
    uint16_t trig_pin;           ///< GPIO Pin untuk Pin TRIG
    TIM_HandleTypeDef* echo_tim; ///< Handle Timer untuk Pin ECHO (Input Capture)
    uint32_t echo_channel;       ///< Channel Timer Input Capture (misal TIM_CHANNEL_1)

    UltrasonicSensorConfig()
        : trig_port(nullptr), trig_pin(0), echo_tim(nullptr), echo_channel(0) {}

    UltrasonicSensorConfig(GPIO_TypeDef* port, uint16_t pin, TIM_HandleTypeDef* tim, uint32_t ch)
        : trig_port(port), trig_pin(pin), echo_tim(tim), echo_channel(ch) {}
};

/**
 * @brief State mesin siklus pembacaan ultrasonik
 */
enum class UltrasonicState : uint8_t {
    IDLE = 0,
    TRIGGERING,
    WAITING_ECHO,
    CYCLE_COMPLETE
};

/**
 * @brief Kelas Driver Pengelola 4 Sensor HC-SR04
 */
class UltrasonicDriver {
public:
    UltrasonicDriver();

    /**
     * @brief Konfigurasi pin hardware untuk ke-4 sensor
     */
    void configure_sensor(UltrasonicIndex index, const UltrasonicSensorConfig& config);

    /**
     * @brief Inisialisasi driver dan mulai timer input capture
     */
    void init();

    /**
     * @brief Update loop non-blocking (panggil secara periodik di main loop / FreeRTOS task)
     * @param current_tick_ms Nilai tick saat ini (HAL_GetTick())
     */
    void update(uint32_t current_tick_ms);

    /**
     * @brief ISR Callback saat rising/falling edge terdeteksi pada Timer Input Capture
     * @param htim Timer handle yang memicu interrupt
     * @param channel Channel timer input capture
     */
    void on_capture_callback(TIM_HandleTypeDef* htim, uint32_t channel);

    /**
     * @brief Ambil jarak terukur untuk sensor tertentu dalam milimeter (mm)
     * @param index Indeks sensor (US_FRONT, US_BACK, US_LEFT, US_RIGHT)
     * @return uint16_t Jarak dalam mm (atau 9999 jika di luar jangkauan)
     */
    uint16_t get_distance_mm(UltrasonicIndex index) const;

    /**
     * @brief Ambil semua 4 jarak sekaligus (Depan, Belakang, Kiri, Kanan)
     */
    void get_all_distances_mm(uint16_t out_distances[NUM_ULTRASONIC_SENSORS]) const;

    /**
     * @brief Cek apakah ada rintangan di bawah ambang batas refleks (< 80 mm)
     * @param index Posisi sensor
     * @param threshold_mm Ambang batas (default 80 mm)
     * @return bool True jika ada rintangan membahayakan
     */
    bool is_obstacle_detected(UltrasonicIndex index, uint16_t threshold_mm = REFLEX_THRESHOLD_MM) const;

    /**
     * @brief Hitung jarak dari lebar pulsa mikrodetik
     * @param duration_us Durasi pulsa HIGH sinyal ECHO (us)
     * @return uint16_t Jarak dalam mm
     */
    static uint16_t duration_us_to_distance_mm(uint32_t duration_us);

    // Untuk keperluan simulasi testing host
    void simulate_pulse_capture(UltrasonicIndex index, uint32_t duration_us);

private:
    void trigger_sensor(UltrasonicIndex index);
    void delay_us_blocking(uint32_t us);

    UltrasonicSensorConfig configs_[NUM_ULTRASONIC_SENSORS];

    volatile uint32_t capture_start_tick_[NUM_ULTRASONIC_SENSORS];
    volatile uint32_t echo_duration_us_[NUM_ULTRASONIC_SENSORS];
    volatile bool is_capturing_[NUM_ULTRASONIC_SENSORS];
    volatile bool new_data_flag_[NUM_ULTRASONIC_SENSORS];

    uint16_t filtered_distance_mm_[NUM_ULTRASONIC_SENSORS];

    uint8_t current_active_sensor_idx_;
    uint32_t last_trigger_tick_ms_;
    UltrasonicState state_;
};

} // namespace Movement

#endif // MOVEMENT_ULTRASONIC_H
