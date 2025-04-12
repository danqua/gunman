#include "entity.h"
#include "game.h"
#include "render/render.h"

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

void DrawEntities(EntityManager* entity_manager)
{
    for (u64 i = 0; i < entity_manager->entity_count; ++i)
    {
        Entity* entity = entity_manager->entities[i];
        Vec2 position = entity->transform.position;
        f32 radius = entity->collider.radius;
        Vec2 forward = EntityGetForward(entity);
        Vec2 target = Vec2Add(position, Vec2Multiply(forward, radius));

        Vec2 min = Vec2Subtract(position, Vec2{ radius, radius });
        Vec2 max = Vec2Add(position, Vec2{ radius, radius });
        Vec2 size = Vec2Subtract(max, min);
        RendererDrawLine2D(position, target, COLOR_YELLOW);
        RendererDrawRect2D(min, size, COLOR_RED);
    }
}

Vec2 EntityGetForward(Entity* entity)
{
    Vec2 result;
    result.x = Cos(entity->transform.angle);
    result.y = Sin(entity->transform.angle);
    return result;
}

b32 EntityHasFlag(Entity* entity, u32 flag)
{
    b32 result = (entity->flags & flag) == flag;
    return result;
}

void EntitySetFlag(Entity* entity, u32 flag)
{
    entity->flags |= flag;
}

void EntityClearFlag(Entity* entity, u32 flag)
{
    entity->flags &= ~flag;
}