#include "level.h"

void LevelInit(Level* level, s32 width, s32 height, Arena* arena)
{
    level->width = width;
    level->height = height;
    level->tiles = (Tile*)Arena_PushSize(arena, sizeof(Tile) * width * height);
}


void LevelClear(Level* level)
{
    for (s32 i = 0; i < level->width * level->height; ++i)
    {
        level->tiles[i] = {};
    }
}

s32 LevelGetSurfaceCount(const Level* level)
{
    s32 surfaceCount = 0;

    for (s32 y = 0; y < level->height; ++y)
    {
        for (s32 x = 0; x < level->width; ++x)
        {
            Tile* current = LevelGetTileAt(level, x, y);
            if (current && current->type == TileType_Empty)
            {
                // Count the floor and ceiling surfaces
                surfaceCount += 2;
                continue;
            }

            s32 xofs[] = { 0,  0, 1, -1 };
            s32 yofs[] = { 1, -1, 0,  0 };
            for (s32 i = 0; i < 4; ++i)
            {
                Tile* tile = LevelGetTileAt(level, x + xofs[i], y + yofs[i]);
                if (tile && tile->type == TileType_Empty)
                {
                    surfaceCount++;
                }
            }
        }
    }

    return surfaceCount;
}

Tile* LevelGetTileAt(const Level* level, s32 x, s32 y)
{
    if (x >= 0 && y >= 0 && x < level->width && y < level->height)
    {
        return &level->tiles[y * level->width + x];
    }
    return nullptr;
}

void LevelSetTileAt(Level* level, s32 x, s32 y, TileType type)
{
    if (x < level->width && y < level->height)
    {
        level->tiles[y * level->width + x].type = type;
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

        Tile* tile = LevelGetTileAt(level, levelPostion.x, levelPostion.y);

        if (tile->type == TileType_Wall)
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
        out->tile = LevelGetTileAt(level, levelPostion.x, levelPostion.y);
    }

    return hit;
}