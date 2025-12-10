#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "core/audio.h"
#include "core/assets.h"
#include "renderer/camera.h"
#include "renderer/renderer.h"
#include "scene/level.h"
#include "game/settings.h"

#include "game/world.h"
#include <glm/glm.hpp>

#define MAX_ENTITIES 128
#define DOOR_OPENING_TIME 0.5f
#define DOOR_OPEN_TIME 3.5f
#define DOOR_CLOSING_TIME 0.5f
#define MAX_KEYS 8


extern AudioId sfxDoorOpen;
extern AudioId sfxPickupItem;

extern Mesh meshDoorLower;
extern Mesh meshDoorUpper;

extern Material materialDefault;

enum EntityType
{
    EntityType_None = 0,
    EntityType_Player,
    EntityType_Enemy,
    EntityType_Pickup,
    EntityType_Door,
    EntityType_Light,
    EntityType_Trigger,
    EntityType_Count
};

struct Player
{
    u32 keys;
    s32 keyCount;
    f32 speed;
    Entity* pointLight;
};

enum PickupType
{
    PickupType_Key
};

struct Pickup
{
    PickupType type;
    s32 data;
    f32 time;
    f32 defaultZ;
    f32 rotationSpeed;
};

enum DoorAxis
{
    DoorAxis_Horizontal,
    DoorAxis_Vertical
};

enum DoorState
{
    DoorState_Closed,
    DoorState_Opening,
    DoorState_Open,
    DoorState_Closing
};

struct Door
{   
    f32 timer;
    DoorState state;
    Entity* lowerDoorEntity;
    Entity* upperDoorEntity;

    DoorAxis axis;

    s32 keyId;


};

struct Trigger
{

};

struct Entity
{
    u16 tileX, tileY;
    f32 radius;

    TransformComponent transform;
    MovementComponent movement;
    ColliderComponent collider;
    MeshRendererComponent meshRenderer;
    LightComponent light;
    CameraComponent camera;
    SpriteAnimationComponent spriteAnimation;

    EntityType type;
    union
    {
        Player player;
        Pickup pickup;
        Door door;
    };
};

struct EntityGridCell
{
    Entity* entities[MAX_ENTITIES_PER_TILE];
    u32 entityCount;
};

struct GameContext
{
    Entity* camera;

    u16 maxEntities;
    u16 entityCount;
    Entity** entities;
    Entity* player;
    
    Level* level;

    EntityGridCell* entityGrid;
    u16 gridWidth;
    u16 gridHeight;

    Pool entityPool;
};

// Initializes the game state with a grid of given dimensions and maximum entities.
GameContext* CreateGameState(Arena* arena, u16 gridWidth, u16 gridHeight, u16 maxEntities);

// Clears the game state and resets the entity pool.
void DestroyGameState(GameContext* state);

// Spawns an entity and registers it in the grid.
Entity* SpawnEntity(GameContext* state, EntityType type, v3 position);

// Destroys an entity and removes it from the grid.
void DestroyEntity(GameContext* state, Entity* entity);

// player.cpp
b32 Player_HasKey(const Entity* player, u32 key);
void Player_AddKey(Entity* player, u32 key);

void MovementSystem(Entity** entities, u32 entityCount, Level* level, f32 deltaTime);
void CollisionSystem(Entity** entities, u32 entityCount, f32 deltaTime);
void AnimationSystem(Entity** entities, u32 entityCount, f32 deltaTime);
void RenderSystem(Entity** entities, u32 entityCount);



struct Game
{
    Arena permanentArena;
    Arena transientArena;
    GameContext* context;
    GameSettings settings;
};

extern Game* game;

void Game_Init(void* userBuffer, u64 userBufferSize);
void Game_Shutdown();
void Game_Update(f32 dt);
void Game_Render();

Entity* CreateDoor(GameContext* context, v3 position, DoorAxis axis);
b32 OpenDoor(GameContext* context, Entity* door);


void UpdatePlayer(GameContext* context, Entity* entity, f32 deltaTime);
void UpdatePickup(GameContext* context, Entity* pickup, f32 deltaTime);
void UpdateDoor(GameContext* context, Entity* door, f32 deltaTime);

void DrawPlayer(GameContext* context, Entity* entity);