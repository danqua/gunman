#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "renderer/rhi.h"
#include <glm/glm.hpp>

enum WallDirection
{
    WallDirection_North,
    WallDirection_East,
    WallDirection_South,
    WallDirection_West
};

enum Layer
{
    Layer_Floor,
    Layer_Ceiling,
    Layer_Wall,
    Layer_Count
};

struct RayCastHit
{
    f32 distance;
    s32 tileX;
    s32 tileY;
    Layer layer;
    glm::vec2 hit;
    glm::vec2 normal;
};

struct Level
{
    s32 width;
    s32 height;
    u32* tiles[Layer_Count];
};

void LevelInit(Level* level, s32 width, s32 height, Arena* arena);
void LevelClear(Level* level);
u32 LevelGetTileAt(const Level* level, s32 x, s32 y, Layer layer);
void LevelSetTileAt(Level* level, s32 x, s32 y, u32 data, Layer layer);
b32 LevelCastRay(const Level* level, glm::vec2 origin, glm::vec2 direction, RayCastHit* out, f32 maxDistance = 128.0f);
