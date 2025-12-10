#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "renderer/rhi.h"
#include "renderer/image.h"
#include <glm/glm.hpp>

#define MAX_ENTITIES_PER_TILE 4

struct Entity;

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

struct Tileset
{
    Image image;
    s32 tileWidth;
    s32 tileHeight;
    s32 tileCount;
    s32 spacing;
    s32 columns;
};

struct Tile
{
    s32 data;
    b32 solid;
    s32 floor;
    s32 ceiling;
    s32 wall;
    Entity* entities[MAX_ENTITIES_PER_TILE];
    s32 entityCount;
};

struct Level
{
    s32 width;
    s32 height;
    Tile* tiles;
};

void Level_Init(Level* level, s32 width, s32 height, Arena* arena);

void Level_Clear(Level* level);

Tile* Level_GetTileAt(const Level* level, s32 x, s32 y);

void Level_SetTileAt(Level* level, s32 x, s32 y, const Tile* tileData);

void Level_AddEntity(Level* level, Entity* entity);

void Level_RemoveEntity(Level* level, Entity* entity);

void Level_UpdateEntityPosition(Level* level, Entity* entity, v2 lastPosition);

b32 Level_CastRay(const Level* level, glm::vec2 origin, glm::vec2 direction, RayCastHit* out, f32 maxDistance = 128.0f);

void Tileset_GetTileUVs(const Tileset* tileset, s32 tileId, glm::vec2* outMin, glm::vec2* outMax);