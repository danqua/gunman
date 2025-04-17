#pragma once
#include "core/types.h"
#include "core/memory.h"

#include "game/entity.h"
#include "game/door.h"

#define MAX_ENTITIES_PER_TILE 8

constexpr u32 kMaxDoors = 32;

enum TileType
{
    TILE_NONE,
    TILE_WALL,
    TILE_DOOR
};

enum TileFlag
{
    TF_NONE = 0,
    TF_TRIGGER = 1 << 0
};

struct Level;
struct Entity;

typedef void(*TileCallback)(Level* level, Entity* entity);

struct Trigger
{
    b32 once;
    b32 active;
    b32 triggered;
    u32 tile_x;
    u32 tile_y;
    Entity* entity;
};

union TileState
{
    Trigger trigger;
};

struct Tile
{
    u32 x;
    u32 y;
    u32 type;
    u32 flags;
    u32 floor_texture;
    u32 ceiling_texture;
    u32 wall_texture;
    u32 door_index;

    TileState state;
    TileCallback on_enter;
    TileCallback on_exit;
    TileCallback on_stay;
};

struct RayCastInfo
{
    Vec2 wall_hit;
    b32 hit;
    Tile* tile;
    f32 perp_distance;
};

struct Level
{
    Arena arena;
    u32 width;
    u32 height;
    Tile* tiles;

    u32 doors_count;
    Door doors[kMaxDoors];

    EntityManager entity_manager;
};

// Initializes a new level.
void LevelInit(Level* level, u32 width, u32 height, Arena* arena);

// Frees the memory used by a level.
void LevelFree(Level* level);

// Updates the level state.
void LevelUpdate(Level* level, f32 dt);

// Sets the tile at the specified position.
void LevelSetTile(Level* level, u32 x, u32 y, u32 tile_type);

// Returns the tile at the specified position.
Tile* LevelGetTile(Level* level, u32 x, u32 y);

// Returns the entity at the specified position.
Entity* LevelGetEntityAt(Level* level, u32 x, u32 y);

// Returns true if the entity can move to the specified position.
b32 LevelEntityCanMoveTo(Level* level, Entity* entity, f32 x, f32 y);

// Returns true if the tile is solid.
b32 IsSolid(Level* level, u32 x, u32 y);

// Returns true if the tile is a trigger.
b32 IsTrigger(Level* level, u32 x, u32 y);

// Casts a ray in the level and returns the hit information.
RayCastInfo LevelCastRay(Level* level, Vec2 origin, Vec2 direction, u32 max_depth = 16);


