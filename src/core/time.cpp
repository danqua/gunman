#include "time.h"

void TimeInit(Time* time, f32 fixed_dt)
{
    time->real_time = 0.0f;
    time->delta_time = 0.0f;
    time->time_scale = 1.0f;

    time->scaled_time = 0.0f;
    time->scaled_dt = 0.0f;

    time->fixed_dt = fixed_dt;
    time->accumulator = 0.0f;
    time->steps = 0;
}

void TimeUpdate(Time* time, f32 now)
{
    static f32 last_time = now;

    time->delta_time = now - last_time;
    time->real_time += time->delta_time;

    time->scaled_dt = time->delta_time * time->time_scale;
    time->scaled_time += time->scaled_dt;

    time->accumulator += time->scaled_dt;
    time->steps = 0;

    last_time = now;
}

bool TimeShouldStep(Time* time)
{
    if (time->accumulator >= time->fixed_dt)
    {
        time->accumulator -= time->fixed_dt;
        time->steps++;
        return true;
    }

    return false;
}