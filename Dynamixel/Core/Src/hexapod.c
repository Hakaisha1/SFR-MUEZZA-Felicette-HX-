#include "hexapod.h"

// Define FreeRTOS Task Handle
osThreadId hexapodControlTaskHandle;
osThreadId hexapodKinematicsTaskHandle;

static GaitPlanner_t gait_planner;
static Dxl_SyncWriteData_t sync_data[18];

// IDs mapping for 18 servos (Coxa, Femur, Tibia per leg)
// 0:RF, 1:LF, 2:RM, 3:LM, 4:RB, 5:LB
static const uint8_t servo_ids[6][3] = {
    {1, 2, 3},       // RF
    {4, 5, 6},       // LF
    {7, 8, 9},       // RM
    {10, 11, 12},    // LM
    {13, 14, 15},    // RB
    {16, 17, 18}     // LB
};

void HexapodControlTask(void const * argument) {
    // Loop interval for gait update (e.g., 20ms = 50Hz)
    const uint32_t TASK_INTERVAL_MS = 20; 
    
    while(1) {
        // Update gait planner
        Gait_Update(&gait_planner, (float)TASK_INTERVAL_MS);
        
        // Calculate IK for all 6 legs and pack into sync_write data
        uint8_t sync_idx = 0;
        for (int i = 0; i < 6; i++) {
            JointAngles_t angles;
            if (CalculateIK(gait_planner.leg_targets[i], &angles)) {
                
                // Pack Coxa
                sync_data[sync_idx].id = servo_ids[i][0];
                sync_data[sync_idx].position = DegToDxl(angles.coxa_angle);
                sync_idx++;
                
                // Pack Femur
                sync_data[sync_idx].id = servo_ids[i][1];
                sync_data[sync_idx].position = DegToDxl(angles.femur_angle);
                sync_idx++;
                
                // Pack Tibia
                sync_data[sync_idx].id = servo_ids[i][2];
                sync_data[sync_idx].position = DegToDxl(angles.tibia_angle);
                sync_idx++;
            }
        }
        
        // Send SyncWrite command to Dynamixel bus
        // Make sure to wrap this in a Mutex if other tasks also access UART
        if (sync_idx == 18) {
            Dxl_SyncWritePosition(sync_data, 18);
        }
        
        // Delay for next cycle
        osDelay(TASK_INTERVAL_MS);
    }
}

void Hexapod_Init(UART_HandleTypeDef *huart) {
    Dxl_Init(huart);
    Gait_Init(&gait_planner);
    
    // Create FreeRTOS Tasks
    osThreadDef(hexapodCtrl, HexapodControlTask, osPriorityNormal, 0, 512);
    hexapodControlTaskHandle = osThreadCreate(osThread(hexapodCtrl), NULL);
}

void Hexapod_SetVelocity(float x_speed, float y_speed, float rot_speed) {
    // Modify gait_planner parameters based on requested velocity
    // E.g., adjust stride_length based on x_speed
}
