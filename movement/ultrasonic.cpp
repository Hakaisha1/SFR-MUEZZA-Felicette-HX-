/**
 * @file ultrasonic.cpp
 * @brief Implementasi Driver 4x Sensor Ultrasonik HC-SR04 (Timer Input-Capture) untuk STM32
 * @author Movement Engineer
 * @date 2026-08-22
 */

#include "ultrasonic.h"
#include <algorithm>

namespace Movement {

UltrasonicDriver::UltrasonicDriver()
    : current_active_sensor_idx_(0),
      last_trigger_tick_ms_(0),
      state_(UltrasonicState::IDLE) {

    for (uint8_t i = 0; i < NUM_ULTRASONIC_SENSORS; ++i) {
        capture_start_tick_[i] = 0;
        echo_duration_us_[i] = 0;
        is_capturing_[i] = false;
        new_data_flag_[i] = false;
        filtered_distance_mm_[i] = OUT_OF_RANGE_DISTANCE_MM;
    }
}

void UltrasonicDriver::configure_sensor(UltrasonicIndex index, const UltrasonicSensorConfig& config) {
    if (index < NUM_ULTRASONIC_SENSORS) {
        configs_[index] = config;
    }
}

void UltrasonicDriver::init() {
    state_ = UltrasonicState::IDLE;
    current_active_sensor_idx_ = 0;
    last_trigger_tick_ms_ = 0;

    for (uint8_t i = 0; i < NUM_ULTRASONIC_SENSORS; ++i) {
        filtered_distance_mm_[i] = OUT_OF_RANGE_DISTANCE_MM;
        is_capturing_[i] = false;
        new_data_flag_[i] = false;

#if ULTRASONIC_STM32_HAL
        if (configs_[i].echo_tim != nullptr) {
            // Start Timer Input Capture dalam mode interrupt
            HAL_TIM_IC_Start_IT(configs_[i].echo_tim, configs_[i].echo_channel);
        }
#endif
    }
}

uint16_t UltrasonicDriver::duration_us_to_distance_mm(uint32_t duration_us) {
    // Kecepatan suara pada udara 20°C = 343 m/s = 0.343 mm/us
    // Jarak (mm) = (durasi_us * 343) / 2000
    if (duration_us == 0) {
        return OUT_OF_RANGE_DISTANCE_MM;
    }

    uint32_t dist_mm = (duration_us * 343UL) / 2000UL;

    if (dist_mm < MIN_VALID_DISTANCE_MM || dist_mm > MAX_VALID_DISTANCE_MM) {
        return OUT_OF_RANGE_DISTANCE_MM;
    }

    return static_cast<uint16_t>(dist_mm);
}

void UltrasonicDriver::delay_us_blocking(uint32_t us) {
#if ULTRASONIC_STM32_HAL
    // Gunakan register DWT (Data Watchpoint and Trace) untuk delay microsecond presisi
    uint32_t start_tick = DWT->CYCCNT;
    uint32_t delay_ticks = us * (SystemCoreClock / 1000000UL);
    while ((DWT->CYCCNT - start_tick) < delay_ticks) {
        __NOP();
    }
#else
    // Dummy loop untuk PC simulation
    volatile uint32_t count = us * 50;
    while (count--) {
        // no-op
    }
#endif
}

void UltrasonicDriver::trigger_sensor(UltrasonicIndex index) {
    if (index >= NUM_ULTRASONIC_SENSORS) return;

#if ULTRASONIC_STM32_HAL
    if (configs_[index].trig_port != nullptr) {
        // Reset state capture untuk sensor ini
        is_capturing_[index] = false;
        new_data_flag_[index] = false;

        // Kirim pulsa HIGH 10 µs pada pin TRIG
        HAL_GPIO_WritePin(configs_[index].trig_port, configs_[index].trig_pin, GPIO_PIN_SET);
        delay_us_blocking(TRIG_PULSE_DURATION_US);
        HAL_GPIO_WritePin(configs_[index].trig_port, configs_[index].trig_pin, GPIO_PIN_RESET);
    }
#endif
}

void UltrasonicDriver::on_capture_callback(TIM_HandleTypeDef* htim, uint32_t channel) {
#if ULTRASONIC_STM32_HAL
    // Cari sensor mana yang cocok dengan htim dan channel ini
    for (uint8_t i = 0; i < NUM_ULTRASONIC_SENSORS; ++i) {
        if (configs_[i].echo_tim == htim && configs_[i].echo_channel == channel) {
            if (!is_capturing_[i]) {
                // RISING EDGE: Catat waktu awal pulsa Echo
                capture_start_tick_[i] = HAL_TIM_ReadCapturedValue(htim, channel);
                is_capturing_[i] = true;

                // Ubah polaritas timer ke Falling Edge untuk menangkap akhir pulsa
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_FALLING);
            } else {
                // FALLING EDGE: Catat waktu akhir pulsa Echo
                uint32_t capture_end = HAL_TIM_ReadCapturedValue(htim, channel);
                
                // Tangani timer counter overflow/wrap-around
                uint32_t duration;
                if (capture_end >= capture_start_tick_[i]) {
                    duration = capture_end - capture_start_tick_[i];
                } else {
                    uint32_t timer_period = __HAL_TIM_GET_AUTORELOAD(htim);
                    duration = (timer_period - capture_start_tick_[i]) + capture_end;
                }

                echo_duration_us_[i] = duration;
                is_capturing_[i] = false;
                new_data_flag_[i] = true;

                // Kembalikan polaritas timer ke Rising Edge untuk pembacaan berikutnya
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_RISING);
            }
            break;
        }
    }
#endif
}

void UltrasonicDriver::update(uint32_t current_tick_ms) {
    if (last_trigger_tick_ms_ == 0) {
        last_trigger_tick_ms_ = current_tick_ms;
        trigger_sensor(static_cast<UltrasonicIndex>(current_active_sensor_idx_));
        state_ = UltrasonicState::WAITING_ECHO;
        return;
    }

    uint32_t elapsed = current_tick_ms - last_trigger_tick_ms_;

    // Pindah dan picu sensor berikutnya jika jeda inter-sensor >= 25ms
    if (elapsed >= INTER_SENSOR_INTERVAL_MS) {
        uint8_t prev_idx = current_active_sensor_idx_;

        // Proses data sensor sebelumnya jika ada data baru
        if (new_data_flag_[prev_idx]) {
            uint16_t raw_dist = duration_us_to_distance_mm(echo_duration_us_[prev_idx]);
            
            // Filter glitch / Simple Moving Average
            if (filtered_distance_mm_[prev_idx] == OUT_OF_RANGE_DISTANCE_MM) {
                filtered_distance_mm_[prev_idx] = raw_dist;
            } else if (raw_dist != OUT_OF_RANGE_DISTANCE_MM) {
                // Low-pass filter: 70% data baru + 30% data lama
                filtered_distance_mm_[prev_idx] = static_cast<uint16_t>((raw_dist * 7 + filtered_distance_mm_[prev_idx] * 3) / 10);
            } else {
                filtered_distance_mm_[prev_idx] = OUT_OF_RANGE_DISTANCE_MM;
            }
            new_data_flag_[prev_idx] = false;
        } else if (elapsed >= (INTER_SENSOR_INTERVAL_MS * 2)) {
            // Timeout: tidak ada echo diterima dalam batas waktu wajar
            filtered_distance_mm_[prev_idx] = OUT_OF_RANGE_DISTANCE_MM;
        }

        // Gilir ke sensor berikutnya (Round-Robin: FRONT -> BACK -> LEFT -> RIGHT)
        current_active_sensor_idx_ = (current_active_sensor_idx_ + 1) % NUM_ULTRASONIC_SENSORS;
        last_trigger_tick_ms_ = current_tick_ms;

        // Picu sensor yang baru aktif
        trigger_sensor(static_cast<UltrasonicIndex>(current_active_sensor_idx_));
    }
}

uint16_t UltrasonicDriver::get_distance_mm(UltrasonicIndex index) const {
    if (index < NUM_ULTRASONIC_SENSORS) {
        return filtered_distance_mm_[index];
    }
    return OUT_OF_RANGE_DISTANCE_MM;
}

void UltrasonicDriver::get_all_distances_mm(uint16_t out_distances[NUM_ULTRASONIC_SENSORS]) const {
    for (uint8_t i = 0; i < NUM_ULTRASONIC_SENSORS; ++i) {
        out_distances[i] = filtered_distance_mm_[i];
    }
}

bool UltrasonicDriver::is_obstacle_detected(UltrasonicIndex index, uint16_t threshold_mm) const {
    if (index < NUM_ULTRASONIC_SENSORS) {
        uint16_t dist = filtered_distance_mm_[index];
        return (dist > 0 && dist < threshold_mm);
    }
    return false;
}

void UltrasonicDriver::simulate_pulse_capture(UltrasonicIndex index, uint32_t duration_us) {
    if (index < NUM_ULTRASONIC_SENSORS) {
        echo_duration_us_[index] = duration_us;
        new_data_flag_[index] = true;
    }
}

} // namespace Movement
