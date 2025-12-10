#include "level.h"
#include "game/game.h"

void Level_Init(Level* level, s32 width, s32 height, Arena* arena)
{
    level->width = width;
    level->height = height;
    level->tiles = ArenaPushArray(arena, Tile, width * height);
}

void Level_Clear(Level* level)
{

}

Tile* Level_GetTileAt(const Level* level, s32 x, s32 y)
{
    if (x >= 0 && y >= 0 && x < level->width && y < level->height)
    {
        return &level->tiles[y * level->width + x];
    }
    return nullptr;
}

void Level_SetTileAt(Level* level, s32 x, s32 y, const Tile* tileData)
{
    if (x < level->width && y < level->height)
    {
        level->tiles[y * level->width + x]= *tileData;
    }
}

void RemoveEntityFromTile(Tile* tile, Entity* entity)
{
    for (s32 i = 0; i < MAX_ENTITIES_PER_TILE; ++i)
    {
        if (tile->entities[i] == entity)
        {
            tile->entities[i] = tile->entities[MAX_ENTITIES_PER_TILE - 1];
            tile->entityCount--;
            break;
        }
    }
}

void Level_AddEntity(Level* level, Entity* entity)
{
    TransformComponent* transform = &entity->transform;
    ColliderComponent* collider = &entity->collider;

    if (!transform->enabled && !collider->enabled) return;

    u16 minX = (u16)(transform->position.x - collider->radius);
    u16 minY = (u16)(transform->position.y - collider->radius);
    u16 maxX = (u16)(transform->position.x + collider->radius);
    u16 maxY = (u16)(transform->position.y + collider->radius);

    for (u16 y = minY; y <= maxY; ++y)
    {
        for (u16 x = minX; x <= maxX; ++x)
        {
            Tile* tile = Level_GetTileAt(level, x, y);
            tile->entities[tile->entityCount++] = entity;
        }
    }
}

void Level_RemoveEntity(Level* level, Entity* entity)
{
    TransformComponent* transform = &entity->transform;
    ColliderComponent* collider = &entity->collider;

    if (!transform->enabled && !collider->enabled) return;

    u16 minX = (u16)(transform->position.x - collider->radius);
    u16 minY = (u16)(transform->position.y - collider->radius);
    u16 maxX = (u16)(transform->position.x + collider->radius);
    u16 maxY = (u16)(transform->position.y + collider->radius);

    for (u16 y = minY; y <= maxY; ++y)
    {
        for (u16 x = minX; x <= maxX; ++x)
        {
            Tile* tile = Level_GetTileAt(level, x, y);
            RemoveEntityFromTile(tile, entity);
        }
    }
}

void Level_UpdateEntityPosition(Level* level, Entity* entity, v2 lastPosition)
{
    TransformComponent* transform = &entity->transform;
    ColliderComponent* collider = &entity->collider;

    if (!transform->enabled && !collider->enabled) return;

    u16 minX = (u16)(lastPosition.x - collider->radius);
    u16 minY = (u16)(lastPosition.y - collider->radius);
    u16 maxX = (u16)(lastPosition.x + collider->radius);
    u16 maxY = (u16)(lastPosition.y + collider->radius);

    for (u16 y = minY; y <= maxY; ++y)
    {
        for (u16 x = minX; x <= maxX; ++x)
        {
            Tile* tile = Level_GetTileAt(level, x, y);
            RemoveEntityFromTile(tile, entity);
        }
    }

    Level_AddEntity(level, entity);
}

b32 Level_CastRay(const Level* level, v2 origin, v2 direction, RayCastHit* out, f32 maxDistance)
{
    glm::ivec2 levelPostion = glm::ivec2(
        (s32)origin.x,
        (s32)origin.y
    );

    v2 deltaDistance = v2(
        (direction.x == 0) ? 1e30f : glm::abs(1.0f / direction.x),
        (direction.y == 0) ? 1e30f : glm::abs(1.0f / direction.y)
    );

    v2 step = v2(
        direction.x > 0.0f ? 1.0f : -1.0f,
        direction.y > 0.0f ? 1.0f : -1.0f
    );

    v2 sideDistance = v2(
        (direction.x > 0.0f ? (levelPostion.x + 1.0f - origin.x) : (origin.x - levelPostion.x)) * deltaDistance.x,
        (direction.y > 0.0f ? (levelPostion.y + 1.0f - origin.y) : (origin.y - levelPostion.y)) * deltaDistance.y
    );

    b32 hit = 0;
    b32 side = 0;
    f32 distance = 0.0f;

    while (!hit && distance <= maxDistance)
    {
        if (sideDistance.x < sideDistance.y)
        {
            sideDistance.x += deltaDistance.x;
            levelPostion.x += (s32)step.x;
            side = 0;
        }
        else
        {
            sideDistance.y += deltaDistance.y;
            levelPostion.y += (s32)step.y;
            side = 1;
        }

        if (levelPostion.x < 0 || levelPostion.x >= level->width ||
            levelPostion.y < 0 || levelPostion.y >= level->height)
        {
            break;
        }

        Tile* tile = Level_GetTileAt(level, levelPostion.x, levelPostion.y);

        if (tile->solid)
        {
            hit = 1;
        }

        distance = side ? sideDistance.y : sideDistance.x;
    }

    if (hit)
    {
        out->distance = side ? sideDistance.y - deltaDistance.y : sideDistance.x - deltaDistance.x;
        out->hit = origin + direction * out->distance;
        out->normal = side ? v2(-1.0f, 0.0f) : v2(0.0f, -1.0f);
        out->tileX = levelPostion.x;
        out->tileY = levelPostion.y;
        out->layer = Layer_Wall;
    }

    return hit;
}