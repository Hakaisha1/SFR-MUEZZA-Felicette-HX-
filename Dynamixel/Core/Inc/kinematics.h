#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdint.h>
#include <math.h>

/* Hexapod Leg Geometry (in millimeters) */
/* Adjust these based on your actual robot physical dimensions */
#define COXA_LENGTH    50.0f
#define FEMUR_LENGTH   70.0f
#define TIBIA_LENGTH   110.0f

/* Math Constants */
#define PI 3.14159265358979323846f

/* Struct to hold Cartesian Coordinates */
typedef struct {
    float x;
    float y;
    float z;
} Point3D_t;

/* Struct to hold Joint Angles (in Degrees or Dynamixel Units) */
typedef struct {
    float coxa_angle;  // degrees
    float femur_angle; // degrees
    float tibia_angle; // degrees
} JointAngles_t;

/* Convert degrees to Dynamixel position (0-1023 for AX-12A) */
// AX-12A resolution is 0.29 degrees per unit. 
// Range: 0 (0 deg) to 1023 (300 deg). Center is 512 (150 deg).
uint16_t DegToDxl(float degrees);

/* Calculate Inverse Kinematics for a single leg */
// Input: Target X, Y, Z coordinates (relative to the coxa joint origin)
// Output: Calculated joint angles in degrees
// Returns: 1 if successful, 0 if coordinates are out of reach
uint8_t CalculateIK(Point3D_t target, JointAngles_t *angles);

#endif // KINEMATICS_H
