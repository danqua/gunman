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
    ENTITY_TRIGGER
};

struct Transform
{
    Vec2 position;
    f32 angle;
};

struct Movement
{
    Vec2 velocity;
    f32 friction;
    f32 speed;
};

struct Collider
{
    f32 radius;
    u32 layer;
    u32 mask;
};

struct Entity
{
    EntityType type;
    Transform transform;
    Movement movement;
    Collider collider;
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

// Updates all entities.
void UpdateEntities(EntityManager* entity_manager, f32 dt);

// Draws all entities.
void DrawEntities(EntityManager* entity_manager);

// Returns the forward direction of an entity.
Vec2 EntityGetForward(Entity* entity);