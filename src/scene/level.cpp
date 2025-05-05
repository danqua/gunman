#include "level.h"

void LevelInit(Level* level, s32 width, s32 height, Arena* arena)
{
    level->width = width;
    level->height = height;

    u64 layerSizeInBytes = sizeof(u32) * width * height;
    level->tiles[Layer_Floor] = (u32*)Arena_PushSize(arena, layerSizeInBytes);
    level->tiles[Layer_Ceiling] = (u32*)Arena_PushSize(arena, layerSizeInBytes);
    level->tiles[Layer_Wall] = (u32*)Arena_PushSize(arena, layerSizeInBytes);
}


void LevelClear(Level* level)
{
    for (s32 i = 0; i < Layer_Count; ++i)
    {
        for (s32 j = 0; j < level->width * level->height; ++j)
        {
            level->tiles[i][j] = {};
        }
    }
}

u32 LevelGetTileAt(const Level* level, s32 x, s32 y, Layer layer)
{
    if (x >= 0 && y >= 0 && x < level->width && y < level->height)
    {
        return level->tiles[layer][y * level->width + x];
    }
    return -1;
}

void LevelSetTileAt(Level* level, s32 x, s32 y, u32 data, Layer layer)
{
    if (x < level->width && y < level->height)
    {
        level->tiles[layer][y * level->width + x]= data;
    }
}

b32 LevelCastRay(const Level* level, glm::vec2 origin, glm::vec2 direction, RayCastHit* out, f32 maxDistance)
{
    glm::ivec2 levelPostion = glm::ivec2(
        (s32)origin.x,
        (s32)origin.y
    );

    glm::vec2 deltaDistance = glm::vec2(
        (direction.x == 0) ? 1e30f : glm::abs(1.0f / direction.x),
        (direction.y == 0) ? 1e30f : glm::abs(1.0f / direction.y)
    );

    glm::vec2 step = glm::vec2(
        direction.x > 0.0f ? 1.0f : -1.0f,
        direction.y > 0.0f ? 1.0f : -1.0f
    );

    glm::vec2 sideDistance = glm::vec2(
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

        u32 tile = LevelGetTileAt(level, levelPostion.x, levelPostion.y, Layer_Wall);

        if (tile)
        {
            hit = 1;
        }

        distance = side ? sideDistance.y : sideDistance.x;
    }

    if (hit)
    {
        out->distance = side ? sideDistance.y - deltaDistance.y : sideDistance.x - deltaDistance.x;
        out->hit = origin + direction * out->distance;
        out->normal = side ? glm::vec2(-1.0f, 0.0f) : glm::vec2(0.0f, -1.0f);
        out->tileX = levelPostion.x;
        out->tileY = levelPostion.y;
        out->layer = Layer_Wall;
    }

    return hit;
}