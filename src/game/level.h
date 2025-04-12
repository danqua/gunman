#pragma once
#include "core/types.h"
#include "core/memory.h"

#include "game/entity.h"

#define MAX_ENTITIES_PER_TILE 8

enum TileType
{
    TILE_NONE,
    TILE_WALL,
    TILE_DOOR
};

struct Tile
{
    u32 x;
    u32 y;
    u32 type;
    u32 floor_texture;
    u32 ceiling_texture;
    u32 wall_texture;
    u32 entity_count;
    Entity* entities[MAX_ENTITIES_PER_TILE];
};

struct RayCastInfo
{
    Vec2 wall_hit;
    b32 hit;
};

struct Level
{
    Arena arena;
    u32 width;
    u32 height;

    u32* tiles;
    Tile* tile_data;

    EntityManager entity_manager;
};

// Initializes a new level.
void LevelInit(Level* level, u32 width, u32 height, Arena* arena);

// Frees the memory used by a level.
void LevelFree(Level* level);

// Sets the tile at the specified position.
void LevelSetTile(Level* level, u32 x, u32 y, u32 tile_type);

// Returns the tile at the specified position.
u32 LevelGetTile(Level* level, u32 x, u32 y);

// Returns true if the entity can move to the specified position.
b32 LevelCanMoveTo(Level* level, Entity* entity, f32 x, f32 y);

// Returns all the tiles occupied by the aabb.
u64 LevelGetTiles(Level* level, Vec2 min, Vec2 max, u32* tiles, u64 tile_count);

// Returns true if the tile is solid.
b32 IsSolid(Level* level, u32 x, u32 y);

// Casts a ray in the level and returns the hit information.
RayCastInfo LevelCastRay(Level* level, Vec2 origin, Vec2 direction, u32 max_depth = 16);