/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

// ============================================================================
// [PLACEHOLDER] PINOUT UNTUK DIVISI ELEKTRIK
// ============================================================================

// --- 1. DYNAMIXEL (MOTOR SERVO) ---
// Note: TX/RX Data (Half-Duplex) sudah fix di PA9 (USART1).
// Jika rangkaian driver Dynamixel (misal 74HC126/MAX485) butuh pin Direction/Enable:
#define DXL_DIR_PORT            GPIOA
#define DXL_DIR_PIN             GPIO_PIN_8

// --- 1b. SERVO MG (END EFFECTOR / CAPIT) ---
// Note: Butuh hardware timer PWM (misal TIM2_CH1). Aktifkan di .ioc nanti.
#define SERVO_GRIP_PORT         GPIOB
#define SERVO_GRIP_PIN          GPIO_PIN_8   // Contoh: TIM2_CH1

// --- 2. SENSOR ULTRASONIK (HC-SR04) ---
// Sensor Depan
#define US_DEPAN_TRIG_PORT      GPIOB
#define US_DEPAN_TRIG_PIN       GPIO_PIN_12
#define US_DEPAN_ECHO_PORT      GPIOA
#define US_DEPAN_ECHO_PIN       GPIO_PIN_10

// Sensor Belakang
#define US_BELAKANG_TRIG_PORT   GPIOB
#define US_BELAKANG_TRIG_PIN    GPIO_PIN_2
#define US_BELAKANG_ECHO_PORT   GPIOB
#define US_BELAKANG_ECHO_PIN    GPIO_PIN_14

// Sensor Kiri
#define US_KIRI_TRIG_PORT       GPIOB
#define US_KIRI_TRIG_PIN        GPIO_PIN_4
#define US_KIRI_ECHO_PORT       GPIOB
#define US_KIRI_ECHO_PIN        GPIO_PIN_13

// Sensor Kanan
#define US_KANAN_TRIG_PORT      GPIOB
#define US_KANAN_TRIG_PIN       GPIO_PIN_6
#define US_KANAN_ECHO_PORT      GPIOA
#define US_KANAN_ECHO_PIN       GPIO_PIN_15

// --- 3. TOMBOL (USER INTERFACE) ---
#define BTN_START_PORT          GPIOB
#define BTN_START_PIN           GPIO_PIN_15   // Contoh: Tombol Start
#define BTN_ESTOP_PORT          GPIOC
#define BTN_ESTOP_PIN           GPIO_PIN_14   // ilangin

// --- 4. BUZZER / INDIKATOR ---
#define BUZZER_PORT             GPIOA
#define BUZZER_PIN              GPIO_PIN_8

// ============================================================================

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
