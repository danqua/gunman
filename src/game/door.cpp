#include "door.h"
#include "level.h"

void DoorOpen(Door* door)
{
    if (door->state != DoorState_Closed)
    {
        return;
    }

    door->state = DoorState_Opening;
    door->timer = 0.0f;
}

b32 DoorClose(Door* door, Level* level)
{
    if (door->state != DoorState_Open)
    {
        return false;
    }

    // Make sure that no entity is inside the door
    Entity* entity = LevelGetEntityAt(level, door->tile_x, door->tile_y);

    if (!entity)
    {
        door->state = DoorState_Closing;
        door->timer = 0.0f;
        return true;
    }

    return false;
}

void DoorUpdate(Door* door, Level* level, f32 dt)
{
    switch (door->state)
    {
        case DoorState_Closed: {} break;

        case DoorState_Open: {
            door->timer += dt;
            if (door->timer >= kDoorOpenDuration)
            {
                if (!DoorClose(door, level))
                {
                    door->timer = 0.0f;
                }
            }
        } break;

        case DoorState_Closing: {
            door->timer += dt;

            if (door->timer >= kDoorTransitionTime)
            {
                door->state = DoorState_Closed;
                door->timer = 0.0f;
            }
        } break;

        case DoorState_Opening: {
            door->timer += dt;

            if (door->timer >= kDoorTransitionTime)
            {
                door->state = DoorState_Open;
                door->timer = 0.0f;
            }
        } break;

        default: {
            // invalid state
            door->state = DoorState_Closed;
            door->timer = 0.0f;
        } break;
    }
}