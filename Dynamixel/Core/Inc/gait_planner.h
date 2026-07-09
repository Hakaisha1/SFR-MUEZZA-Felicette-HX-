#ifndef GAIT_PLANNER_H
#define GAIT_PLANNER_H

#include "kinematics.h"

/* Gait Types */
typedef enum {
    GAIT_TRIPOD,
    GAIT_RIPPLE,
    GAIT_WAVE
} GaitType_t;

/* Gait State */
typedef struct {
    float stride_length; // X direction length
    float stride_width;  // Y direction length
    float step_height;   // Z direction height during swing
    float cycle_time;    // Total time for one full gait cycle (ms)
    float current_time;  // Current time in the cycle (ms)
    GaitType_t type;
    
    // Output: target positions for all 6 legs
    Point3D_t leg_targets[6]; 
} GaitPlanner_t;

void Gait_Init(GaitPlanner_t *planner);
void Gait_SetSpeed(GaitPlanner_t *planner, float speed);
void Gait_SetDirection(GaitPlanner_t *planner, float angle);

/* Update the gait planner state given a time increment (delta_t in ms) */
/* This function calculates the new Cartesian coordinates for each of the 6 legs */
void Gait_Update(GaitPlanner_t *planner, float delta_t_ms);

#endif // GAIT_PLANNER_H
