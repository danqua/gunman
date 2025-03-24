#pragma once
#include "core/types.h"

struct Time
{
    f32 real_time;      // Time since game starts.
    f32 delta_time;     // Time since last frame.
    f32 time_scale;     // Time scale (1.0f = normal, 0.5f = slow motion, 0 = paused).

    f32 scaled_time;    // Scaled time since game starts.
    f32 scaled_dt;      // Scaled time since last frame.

    f32 fixed_dt;       // Fixed time step.
    f32 accumulator;    // Accumulator for fixed time step.
    s32 steps;          // Number of fixed steps this frame.
};

// Initializes the time system.
void TimeInit(Time* time, f32 fixed_dt = 1.0f / 60.0f);

// Updates the time.
void TimeUpdate(Time* time, f32 now);

// Call repeatedly in fixed update loop.
bool TimeShouldStep(Time* time);