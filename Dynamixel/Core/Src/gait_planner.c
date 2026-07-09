#include "gait_planner.h"
#include <math.h>

void Gait_Init(GaitPlanner_t *planner) {
    planner->stride_length = 50.0f; // 50mm
    planner->stride_width = 0.0f;
    planner->step_height = 40.0f;   // 40mm swing height
    planner->cycle_time = 1000.0f;  // 1 second per cycle
    planner->current_time = 0.0f;
    planner->type = GAIT_TRIPOD;
    
    // Default rest positions for each leg
    for (int i = 0; i < 6; i++) {
        planner->leg_targets[i].x = 100.0f;
        planner->leg_targets[i].y = 0.0f;
        planner->leg_targets[i].z = -100.0f; // Default height from ground to coxa
    }
}

void Gait_Update(GaitPlanner_t *planner, float delta_t_ms) {
    planner->current_time += delta_t_ms;
    if (planner->current_time >= planner->cycle_time) {
        planner->current_time -= planner->cycle_time;
    }
    
    // Normalized time [0.0 to 1.0)
    float t_norm = planner->current_time / planner->cycle_time;
    
    if (planner->type == GAIT_TRIPOD) {
        // Tripod Gait: 
        // Group A: Legs 0, 3, 4 (Right Front, Left Middle, Right Back)
        // Group B: Legs 1, 2, 5 (Left Front, Right Middle, Left Back)
        
        float t_group_a = t_norm;
        float t_group_b = t_norm + 0.5f;
        if (t_group_b >= 1.0f) t_group_b -= 1.0f;
        
        for (int i = 0; i < 6; i++) {
            float phase = (i == 0 || i == 3 || i == 4) ? t_group_a : t_group_b;
            
            // Swing phase (0 to 0.5) and Stance phase (0.5 to 1.0)
            if (phase < 0.5f) { // Swing
                float swing_t = phase * 2.0f; // 0.0 to 1.0 during swing
                
                // Parabolic trajectory for Z
                planner->leg_targets[i].z = -100.0f + planner->step_height * sinf(swing_t * PI);
                
                // Linear trajectory for X (moving forward)
                planner->leg_targets[i].x = 100.0f - (planner->stride_length / 2.0f) + (planner->stride_length * swing_t);
            } else { // Stance
                float stance_t = (phase - 0.5f) * 2.0f; // 0.0 to 1.0 during stance
                
                planner->leg_targets[i].z = -100.0f; // On the ground
                
                // Linear trajectory for X (pushing backward to propel robot forward)
                planner->leg_targets[i].x = 100.0f + (planner->stride_length / 2.0f) - (planner->stride_length * stance_t);
            }
        }
    }
}
