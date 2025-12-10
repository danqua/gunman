#include "game.h"

void UpdatePickup(GameContext* context, Entity* entity, f32 deltaTime)
{
    Pickup* pickup = &entity->pickup;

    pickup->time += deltaTime;

    entity->transform.rotation.z += pickup->rotationSpeed * deltaTime;
    entity->transform.position.z = pickup->defaultZ + Sin(pickup->time) * 0.05f;
}