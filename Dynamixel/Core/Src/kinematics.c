#include "kinematics.h"

uint16_t DegToDxl(float degrees) {
    // AX-12A 0 degrees is typically 512 (150 degrees physically) if we center it.
    // Let's assume standard hexapod configuration where 150 deg is the middle position (value 512)
    // 0 to 300 degrees mapped to 0 to 1023
    
    // Offset by 150 so 0 degree input is the center of the servo
    float shifted_deg = degrees + 150.0f; 
    
    if (shifted_deg < 0.0f) shifted_deg = 0.0f;
    if (shifted_deg > 300.0f) shifted_deg = 300.0f;
    
    float dxl_val = (shifted_deg / 300.0f) * 1023.0f;
    return (uint16_t)dxl_val;
}

uint8_t CalculateIK(Point3D_t target, JointAngles_t *angles) {
    // 1. Calculate Coxa Angle (Rotation around Z axis)
    // Looking from top, X is forward, Y is sideways
    angles->coxa_angle = atan2f(target.x, target.y) * (180.0f / PI);
    
    // 2. Calculate Distance from Coxa joint to Foot in the XY plane
    float L = sqrtf(target.x * target.x + target.y * target.y) - COXA_LENGTH;
    
    // 3. Calculate distance from Femur joint to Foot (hypotenuse)
    float d = sqrtf(L * L + target.z * target.z);
    
    // Check if target is reachable
    if (d > (FEMUR_LENGTH + TIBIA_LENGTH)) {
        return 0; // Target is too far
    }
    
    // 4. Calculate Femur Angle (using Law of Cosines)
    // Angle alpha is the angle between L and d
    float alpha = atan2f(target.z, L);
    
    // Angle beta is the angle inside the triangle at the femur joint
    float cos_beta = (FEMUR_LENGTH * FEMUR_LENGTH + d * d - TIBIA_LENGTH * TIBIA_LENGTH) / (2 * FEMUR_LENGTH * d);
    // Clamp to prevent floating point errors causing acos to fail
    if (cos_beta > 1.0f) cos_beta = 1.0f;
    if (cos_beta < -1.0f) cos_beta = -1.0f;
    
    float beta = acosf(cos_beta);
    
    // Femur angle is alpha + beta
    angles->femur_angle = (alpha + beta) * (180.0f / PI);
    
    // 5. Calculate Tibia Angle (using Law of Cosines)
    float cos_gamma = (FEMUR_LENGTH * FEMUR_LENGTH + TIBIA_LENGTH * TIBIA_LENGTH - d * d) / (2 * FEMUR_LENGTH * TIBIA_LENGTH);
    if (cos_gamma > 1.0f) cos_gamma = 1.0f;
    if (cos_gamma < -1.0f) cos_gamma = -1.0f;
    
    float gamma = acosf(cos_gamma);
    
    // Depending on physical mounting, you might need to offset gamma
    // Usually tibia angle is (gamma - 90 degrees) or similar relative to femur
    angles->tibia_angle = (gamma * (180.0f / PI)) - 90.0f;
    
    return 1; // Success
}
