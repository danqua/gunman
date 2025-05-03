#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "renderer/rhi.h"
#include <glm/glm.hpp>

enum TileType
{
    TileType_Empty,
    TileType_Wall,
};

enum WallDirection
{
    WallDirection_North,
    WallDirection_East,
    WallDirection_South,
    WallDirection_West
};

struct Tile
{
    TileType type;
    TextureId wallTexture;
    TextureId floorTexture;
    TextureId ceilingTexture;
};

struct RayCastHit
{
    Tile* tile;
    float distance;
    glm::vec2 hit;
    glm::vec2 normal;
};

struct Level
{
    s32 width;
    s32 height;
    Tile* tiles;
};


void LevelInit(Level* level, s32 width, s32 height, Arena* arena);
void LevelClear(Level* level);
s32 LevelGetSurfaceCount(const Level* level);
Tile* LevelGetTileAt(const Level* level, s32 x, s32 y);
void LevelSetTileAt(Level* level, s32 x, s32 y, TileType type);
b32 LevelCastRay(const Level* level, glm::vec2 origin, glm::vec2 direction, RayCastHit* out, f32 maxDistance = 128.0f);
