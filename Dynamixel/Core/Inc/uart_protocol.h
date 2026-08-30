#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

/* Header Identifier */
#define UART_HEADER_1 0xAA
#define UART_HEADER_2 0x55

/* Command IDs (Receive from RPi5) */
#define CMD_GERAK_OMNIDIRECTIONAL 0x10
#define CMD_MANIPULATOR           0x11
#define CMD_STATE_CONTROL         0x12

/* Telemetry IDs (Send to RPi5) */
#define TLM_ROBOT_STATUS          0x20
#define TLM_SENSOR_DATA           0x21

/* State Enums */
typedef enum {
    STATE_IDLE = 0,
    STATE_WALKING = 1,
    STATE_EVACUATING = 2,
    STATE_EMERGENCY_STOP = 3
} RobotState_t;

/* --- Packed Structs for Payloads --- */
#pragma pack(push, 1)

// CMD 0x10
typedef struct {
    float kecepatan_x;   // m/s
    float kecepatan_y;   // m/s
    float kecepatan_yaw; // rad/s
} CmdGerakOmni_t;

// CMD 0x11
typedef struct {
    float sudut_lengan_1;
    float sudut_lengan_2;
    uint8_t status_gripper; // 0=Buka, 1=Tutup Kuat, 2=Tutup Lembut
} CmdManipulator_t;

// CMD 0x12
typedef struct {
    uint8_t state_target; 
} CmdStateControl_t;

// TLM 0x20
typedef struct {
    uint8_t state_sekarang;
    float tegangan_baterai;
    uint8_t status_hardware_error; // Bitmask
} TlmRobotStatus_t;

// TLM 0x21
typedef struct {
    uint16_t us_depan_mm;
    uint16_t us_belakang_mm;
    uint16_t us_kiri_mm;
    uint16_t us_kanan_mm;
    float imu_roll;
    float imu_pitch;
} TlmSensorData_t;

#pragma pack(pop)

/* --- Callbacks yang perlu di-implementasikan oleh Movement --- */
extern void UartProtocol_OnCmdGerak(const CmdGerakOmni_t* cmd);
extern void UartProtocol_OnCmdManipulator(const CmdManipulator_t* cmd);
extern void UartProtocol_OnCmdStateControl(const CmdStateControl_t* cmd);

/* --- Parser API --- */
/**
 * @brief Fungsi untuk mem-parse byte masuk secara non-blocking (state machine).
 * Panggil ini dari dalam UART RX Interrupt atau Polling loop.
 * Akan otomatis memanggil callback jika paket valid.
 */
void UartProtocol_ParseByte(uint8_t byte);

/* --- Telemetry API --- */
/**
 * @brief Kirim TLM_ROBOT_STATUS ke RPi5.
 */
void UartProtocol_SendStatus(const TlmRobotStatus_t* status);

/**
 * @brief Kirim TLM_SENSOR_DATA ke RPi5.
 */
void UartProtocol_SendSensor(const TlmSensorData_t* sensor);

/**
 * @brief Internal function to actually send bytes via Hardware UART.
 * Wajib di-implementasikan oleh user sesuai HAL/platform.
 */
extern void UartProtocol_TransmitBytes(uint8_t* data, uint16_t length);

#endif // UART_PROTOCOL_H
