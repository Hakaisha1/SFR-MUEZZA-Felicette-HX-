/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Robot SAR Hexapod Controller
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dynamixel.h"
#include "dynamixel_config.h"
#include "uart_protocol.h"
#include "kinematics.h"
#include "gait_planner.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Buffer 1 byte penerimaan UART interrupt */
uint8_t rx_byte_buffer;

/* Subsystem state & gait variables */
static GaitPlanner_t gait_planner;
static Dxl_SyncWriteData_t sync_data[18];
static RobotState_t current_robot_state = STATE_IDLE;

static float current_vx = 0.0f;
static float current_vy = 0.0f;
static float current_vyaw = 0.0f;

static float manipulator_arm1 = 0.0f;
static float manipulator_arm2 = 0.0f;
static uint8_t manipulator_gripper = 0;

static uint32_t last_gait_tick = 0;
static uint32_t last_telemetry_tick = 0;
static uint32_t last_cdc_tick = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* ========================================================================== */
/*                   IMPLEMENTASI PROTOKOL UART & CALLBACK                    */
/* ========================================================================== */

/**
 * @brief Callback saat menerima perintah gerak omnidirectional dari RPi5 (CMD 0x10)
 */
void UartProtocol_OnCmdGerak(const CmdGerakOmni_t* cmd) {
    current_vx = cmd->kecepatan_x;
    current_vy = cmd->kecepatan_y;
    current_vyaw = cmd->kecepatan_yaw;

    // Suplai parameter ke Gait Planner
    gait_planner.stride_length = current_vx * 50.0f;
    gait_planner.stride_width  = current_vy * 30.0f;
}

/**
 * @brief Callback saat menerima perintah perubahan state dari RPi5 (CMD 0x12)
 */
void UartProtocol_OnCmdStateControl(const CmdStateControl_t* cmd) {
    current_robot_state = (RobotState_t)cmd->state_target;

    if (current_robot_state == STATE_EMERGENCY_STOP) {
        // Hentikan kecepatan seketika
        current_vx = 0.0f;
        current_vy = 0.0f;
        current_vyaw = 0.0f;
        gait_planner.stride_length = 0.0f;
        gait_planner.stride_width = 0.0f;

        // Matikan torsi servo untuk keselamatan
        for (int i = 0; i < NUM_LEGS; i++) {
            Dxl_TorqueEnable(DXL_ID_MAP[i].coxa, false);
            Dxl_TorqueEnable(DXL_ID_MAP[i].femur, false);
            Dxl_TorqueEnable(DXL_ID_MAP[i].tibia, false);
        }
    } else if (current_robot_state == STATE_WALKING || current_robot_state == STATE_IDLE) {
        // Aktifkan torsi seluruh servo
        for (int i = 0; i < NUM_LEGS; i++) {
            Dxl_TorqueEnable(DXL_ID_MAP[i].coxa, true);
            Dxl_TorqueEnable(DXL_ID_MAP[i].femur, true);
            Dxl_TorqueEnable(DXL_ID_MAP[i].tibia, true);
        }
    }
}

/**
 * @brief Callback saat menerima perintah manipulator dari RPi5 (CMD 0x11)
 */
void UartProtocol_OnCmdManipulator(const CmdManipulator_t* cmd) {
    manipulator_arm1 = cmd->sudut_lengan_1;
    manipulator_arm2 = cmd->sudut_lengan_2;
    manipulator_gripper = cmd->status_gripper;

    // Kontrol servo gripper jika terpasang
    if (DXL_ID_GRIPPER != 0) {
        uint16_t grip_pos = (manipulator_gripper == 0) ? 300 : 700;
        Dxl_SetGoalPosition(DXL_ID_GRIPPER, grip_pos);
    }
}

/**
 * @brief Implementasi transmisi hardware UART untuk protokol telemetri
 */
void UartProtocol_TransmitBytes(uint8_t* data, uint16_t length) {
    HAL_UART_Transmit(&huart1, data, length, 100);
}

/**
 * @brief Callback interupsi UART RX saat menerima byte dari RPi5
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // 1. Kirim byte ke mesin parser non-blocking
        UartProtocol_ParseByte(rx_byte_buffer);

        // 2. Aktifkan kembali interupsi penerimaan byte berikutnya
        HAL_UART_Receive_IT(huart, &rx_byte_buffer, 1);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 2 */
  // 1. Inisialisasi Driver Dynamixel pada USART1
  Dxl_Init(&huart1);

  // 2. Aktifkan torsi untuk seluruh 18 servo kaki
  for (int i = 0; i < NUM_LEGS; i++) {
      Dxl_TorqueEnable(DXL_ID_MAP[i].coxa, true);
      Dxl_TorqueEnable(DXL_ID_MAP[i].femur, true);
      Dxl_TorqueEnable(DXL_ID_MAP[i].tibia, true);
  }

  // 3. Inisialisasi Tripod Gait Planner
  Gait_Init(&gait_planner);

  // 4. Mulai interupsi UART RX untuk menerima command dari RPi5
  HAL_UART_Receive_IT(&huart1, &rx_byte_buffer, 1);

  last_gait_tick = HAL_GetTick();
  last_telemetry_tick = HAL_GetTick();
  last_cdc_tick = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t now = HAL_GetTick();

    // ------------------------------------------------------------------------
    // Loop 1: Gait Engine & Kinematics Update (50 Hz / Interval 20ms)
    // ------------------------------------------------------------------------
    if ((now - last_gait_tick) >= 20) {
        float dt = (float)(now - last_gait_tick);
        last_gait_tick = now;

        if (current_robot_state == STATE_WALKING || current_robot_state == STATE_EVACUATING) {
            Gait_Update(&gait_planner, dt);

            uint8_t sync_idx = 0;
            for (int i = 0; i < NUM_LEGS; i++) {
                JointAngles_t angles;
                if (CalculateIK(gait_planner.leg_targets[i], &angles)) {
                    // Coxa
                    sync_data[sync_idx].id = DXL_ID_MAP[i].coxa;
                    sync_data[sync_idx].position = DegToDxl(angles.coxa_angle);
                    sync_idx++;

                    // Femur
                    sync_data[sync_idx].id = DXL_ID_MAP[i].femur;
                    sync_data[sync_idx].position = DegToDxl(angles.femur_angle);
                    sync_idx++;

                    // Tibia
                    sync_data[sync_idx].id = DXL_ID_MAP[i].tibia;
                    sync_data[sync_idx].position = DegToDxl(angles.tibia_angle);
                    sync_idx++;
                }
            }

            // Kirim SyncWrite ke 18 servo sekaligus via USART1
            if (sync_idx == 18) {
                Dxl_SyncWritePosition(sync_data, 18);
            }
        }
    }

    // ------------------------------------------------------------------------
    // Loop 2: Pengiriman Telemetri ke RPi5 (20 Hz / Interval 50ms)
    // ------------------------------------------------------------------------
    if ((now - last_telemetry_tick) >= 50) {
        last_telemetry_tick = now;

        // 1. Kirim Status Robot
        TlmRobotStatus_t status = {0};
        status.state_sekarang = (uint8_t)current_robot_state;
        status.tegangan_baterai = 11.8f;
        status.status_hardware_error = 0;
        UartProtocol_SendStatus(&status);

        // 2. Kirim Sensor Data
        TlmSensorData_t sensor = {0};
        sensor.us_depan_mm = 1500;
        sensor.us_belakang_mm = 2000;
        sensor.us_kiri_mm = 9999;
        sensor.us_kanan_mm = 9999;
        sensor.imu_roll = 0.0f;
        sensor.imu_pitch = 0.0f;
        UartProtocol_SendSensor(&sensor);
    }

    // ------------------------------------------------------------------------
    // Loop 3: Monitor Diagnostik Virtual COM Port USB (1 Hz / Interval 1000ms)
    // ------------------------------------------------------------------------
    if ((now - last_cdc_tick) >= 1000) {
        last_cdc_tick = now;

        char dbg[128];
        const char *state_str = "IDLE";
        if (current_robot_state == STATE_WALKING) state_str = "WALKING";
        else if (current_robot_state == STATE_EVACUATING) state_str = "EVACUATING";
        else if (current_robot_state == STATE_EMERGENCY_STOP) state_str = "ESTOP";

        snprintf(dbg, sizeof(dbg), "[SAR-STM32] State: %s | Vx: %.2f Vy: %.2f | Batt: 11.8V\r\n",
                 state_str, current_vx, current_vy);
        CDC_Transmit_FS((uint8_t*)dbg, strlen(dbg));
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 0;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 65535;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 0;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 3599;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 1000000; // Disesuaikan untuk AX-12A (1 Mbps)
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
