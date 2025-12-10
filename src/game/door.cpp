#include "game.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/easing.hpp>

void OnDoorTriggerEnter(GameContext* context, Entity* doorEntity, Entity* otherEntity)
{
    if (otherEntity->type == EntityType_Player)
    {
        OpenDoor(context, doorEntity);
    }
}

void OnDoorTriggerExit(GameContext* context, Entity* doorEntity, Entity* otherEntity)
{
}

Entity* CreateDoor(GameContext* context, v3 position, DoorAxis axis)
{
    Entity* lowerDoorEntity = SpawnEntity(context, EntityType_None, position);
    lowerDoorEntity->meshRenderer.enabled = true;
    lowerDoorEntity->meshRenderer.mesh = &meshDoorLower;
    lowerDoorEntity->meshRenderer.material = &materialDefault;

    lowerDoorEntity->transform.position.x += 0.5f;
    lowerDoorEntity->transform.position.z += 0.5f;

    Entity* upperDoorEntity = SpawnEntity(context, EntityType_None, position);
    upperDoorEntity->meshRenderer.enabled = true;
    upperDoorEntity->meshRenderer.mesh = &meshDoorUpper;
    upperDoorEntity->meshRenderer.material = &materialDefault;

    upperDoorEntity->transform.position.x += 0.5f;
    upperDoorEntity->transform.position.z += 0.5f;

    if (axis == DoorAxis_Vertical)
    {
        lowerDoorEntity->transform.rotation.y = 90.0f;
        upperDoorEntity->transform.rotation.y = 90.0f;
    }

    Entity* doorEntity = SpawnEntity(context, EntityType_Door, position);
    doorEntity->door.lowerDoorEntity = lowerDoorEntity;
    doorEntity->door.upperDoorEntity = upperDoorEntity;
    
    doorEntity->collider.enabled = true;
    doorEntity->collider.type = ColliderType_Box;
    doorEntity->collider.aabb.min = v3(0.0f, 0.0f, 0.0f);
    doorEntity->collider.aabb.max = v3(1.0f, 1.0f, 1.0f);
    doorEntity->collider.isTrigger = true;
    doorEntity->collider.onTriggerEnter = OnDoorTriggerEnter;
    doorEntity->collider.onTriggerExit = OnDoorTriggerExit;

    return doorEntity;
}

void UpdateDoor(GameContext* context, Entity* doorEntity, f32 deltaTime)
{
    Door* door = &doorEntity->door;
    Entity* lowerDoorEntity = door->lowerDoorEntity;
    Entity* upperDoorEntity = door->upperDoorEntity;

    if (lowerDoorEntity == nullptr || upperDoorEntity == nullptr)
    {
        // If the door entities are not set, we cannot update the door
        return;
    }

    f32 openingTime = 1.0f;


    switch (door->state)
    {
        case DoorState_Opening: {
            door->timer += deltaTime;

            f32 t = glm::quadraticEaseOut(glm::clamp(door->timer / openingTime, 0.0f, 1.0f));
            
            lowerDoorEntity->transform.position.y = Lerp(0.0f, -0.32f, t);
            upperDoorEntity->transform.position.y = Lerp(0.0f, 0.7f, t);

            if (door->timer >= openingTime)
            {
                door->timer = 0.0f;
                door->state = DoorState_Open;
            }
        } break;

        case DoorState_Open: {
            door->timer += deltaTime;

            if (doorEntity->collider.currentEntity)
            {
                door->timer = 0.0f;
            }

            if (door->timer > 2.5f)
            {
                Audio_PlayClip(sfxDoorOpen, 1.0f, false);
                door->state = DoorState_Closing;
                door->timer = 0.0f;
            }
        } break;

        case DoorState_Closing: {
            door->timer += deltaTime;

            f32 t = glm::quadraticEaseOut(glm::clamp(door->timer / openingTime, 0.0f, 1.0f));

            lowerDoorEntity->transform.position.y = Lerp(-0.32f, 0.0f, t);
            upperDoorEntity->transform.position.y = Lerp(0.7f, 0.0f, t);

            if (door->timer >= openingTime)
            {
                door->state = DoorState_Closed;
                door->timer = 0.0f;
            }
        } break;

        case DoorState_Closed: {
            // The door is closed, do nothing
        } break;
    }
}


b32 OpenDoor(GameContext* context, Entity* doorEntity)
{
    if (doorEntity->door.state != DoorState_Closed)
        return false;

    doorEntity->door.state = DoorState_Opening;
    doorEntity->door.timer = 0.0f;
    Audio_PlayClip(sfxDoorOpen, 1.0f, false);

    return true;
}