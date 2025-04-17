#pragma once
#include "core/types.h"
#include "core/math.h"
#include "core/memory.h"

#define MAX_ENTITIES 128

enum EntityType
{
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_ITEM,
    ENTITY_DOOR,
    ENTITY_TRIGGER
};

struct Transform
{
    Vec2 position;      // World position.
    Vec2 last_position; // Last frame position.
    f32 angle;          // Rotation in degrees.
};

struct Movement
{
    Vec2 velocity;  // Linear velocity.
    f32 friction;   // Linear friction (1.0 = no friction).
    f32 speed;      // Linear speed.
};

void UpdateTransformMovement(Transform* transform, Movement* movement, f32 dt);

enum CollisionLayer
{
    LAYER_DEFAULT = 1 << 0,
    LAYER_PLAYER  = 1 << 1,
    LAYER_ENEMY   = 1 << 2,
    LAYER_ITEM    = 1 << 3,
    LAYER_TRIGGER = 1 << 4
};

struct Entity;

struct Collider
{
    f32 radius;
    u32 layer;
    u32 mask;

    void(*on_collision)(Entity* entity, Entity* other);
};

enum EntityFlag
{
    EF_NONE       = 0,
    EF_COLLIDABLE = 1 << 0,
    EF_HAS_MOVED  = 1 << 1,
};

struct Entity
{
    EntityType type;
    Transform transform;
    Movement movement;
    Collider collider;
    u32 flags;

    u32 tile_x;
    u32 tile_y;
    u32 last_tile_x;
    u32 last_tile_y;
};

struct EntityManager
{
    Pool pool;          // Pool allocator used to allocate entities.
    Entity** entities;  // Array of active entities.
    u64 entity_count;   // Number of active entities.
};

// Initializes the entity manager.
void EntityManagerInit(EntityManager* entity_manager, Arena* arena);

// Spawns a new entity.
Entity* SpawnEntity(EntityManager* entity_manager);

// Destroys an entity.
void DestroyEntity(EntityManager* entity_manager, Entity* entity);

// Update all entities.
void UpdateEntities(EntityManager* entity_manager, f32 dt);

// Draws all entities.
void DrawEntities(EntityManager* entity_manager);

// Returns the forward direction of an entity.
Vec2 EntityGetForward(Entity* entity);

// Returns true if the entity has a flag.
b32 EntityHasFlag(Entity* entity, u32 flag);

// Sets a flag on the entity.
void EntitySetFlag(Entity* entity, u32 flag);

// Clears a flag on the entity.
void EntityClearFlag(Entity* entity, u32 flag);

// Returns the AABB of the entity.
Box2 EntityGetAABB(Entity* entity);