#pragma once
#include "core/types.h"

constexpr f32 kDoorTransitionTime = 0.5f;
constexpr f32 kDoorOpenDuration = 5.0f;

struct Level;

enum DoorAxis
{
    DoorAxis_None,
    DoorAxis_Horizontal,
    DoorAxis_Vertical
};

enum DoorState
{
    DoorState_Closed,
    DoorState_Closing,
    DoorState_Open,
    DoorState_Opening
};

struct Door
{
    u32 tile_x;
    u32 tile_y;
    f32 timer;
    DoorAxis axis;
    DoorState state;
};

void DoorOpen(Door* door);
b32 DoorClose(Door* door, Level* level);
void DoorUpdate(Door* door, Level* level, f32 dt);