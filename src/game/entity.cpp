#include "entity.h"
#include "render/render.h"

static void UpdateMovement(Transform* transform, Movement* movement, f32 dt)
{
    Vec2 amount = Vec2Multiply(movement->velocity, movement->speed * dt);
    transform->position = Vec2Add(transform->position, amount);
    movement->velocity = Vec2Multiply(movement->velocity, movement->friction);
    if (Abs(movement->velocity.x) < 1e-4f) movement->velocity.x = 0.0f;
    if (Abs(movement->velocity.y) < 1e-4f) movement->velocity.y = 0.0f;
}

void EntityManagerInit(EntityManager* entity_manager, Arena* arena)
{
    entity_manager->entities = (Entity**)ArenaPushSize(arena, MAX_ENTITIES * sizeof(Entity*));
    entity_manager->entity_count = 0;
    PoolInit(&entity_manager->pool, sizeof(Entity), MAX_ENTITIES, ArenaPushSize(arena, MAX_ENTITIES * sizeof(Entity)));
}

Entity* SpawnEntity(EntityManager* entity_manager)
{
    Entity* entity = (Entity*)PoolAllocate(&entity_manager->pool);
    entity_manager->entities[entity_manager->entity_count] = entity;
    entity_manager->entity_count++;
    return entity;
}

void DestroyEntity(EntityManager* entity_manager, Entity* entity)
{
    for (u64 i = 0; i < entity_manager->entity_count; ++i)
    {
        if (entity_manager->entities[i] == entity)
        {
            PoolFree(&entity_manager->pool, entity);
            entity_manager->entities[i] = entity_manager->entities[entity_manager->entity_count - 1];
            entity_manager->entity_count--;
            break;
        }
    }
}

void UpdateEntities(EntityManager* entity_manager, f32 dt)
{
    for (u64 i = 0; i < entity_manager->entity_count; ++i)
    {
        Entity* entity = entity_manager->entities[i];

        switch (entity->type)
        {
            case ENTITY_PLAYER: {
                UpdateMovement(&entity->transform, &entity->movement, dt);
            } break;
        }
    }
}

void DrawEntities(EntityManager* entity_manager)
{
    for (u64 i = 0; i < entity_manager->entity_count; ++i)
    {
        Entity* entity = entity_manager->entities[i];
        Vec2 position = entity->transform.position;
        f32 radius = entity->collider.radius;
        Vec2 forward = EntityGetForward(entity);
        Vec2 target = Vec2Add(position, Vec2Multiply(forward, radius));
        RendererDrawLine2D(position, target, COLOR_YELLOW);
        RendererDrawCircle2D(position, radius, COLOR_GREEN);
    }
}

Vec2 EntityGetForward(Entity* entity)
{
    Vec2 result;
    result.x = Cos(entity->transform.angle);
    result.y = Sin(entity->transform.angle);
    return result;
}