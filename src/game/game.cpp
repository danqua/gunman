#include "game.h"
#include "core/platform.h"
#include "core/input.h"
#include "core/math.h"

Game* game;

AudioId sfxDoorOpen;
AudioId sfxPickupItem;

Mesh meshDoorLower;
Mesh meshDoorUpper;

Material materialDefault;

GameContext* CreateGamecontext(Arena* arena, u16 gridWidth, u16 gridHeight, u16 maxEntities)
{
    GameContext* context = ArenaPushType(arena, GameContext);
    context->entities = ArenaPushArray(arena, Entity*, maxEntities);
    context->entityGrid = ArenaPushArray(arena, EntityGridCell, gridWidth * gridHeight);
    context->gridWidth = gridWidth;
    context->gridHeight = gridHeight;
    context->entityCount = 0;
    context->maxEntities = maxEntities;

    //context->camera = Camera_CreatePerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    void* poolBuffer = Arena_PushSize(arena, maxEntities * sizeof(Entity));
    Pool_Init(&context->entityPool, sizeof(Entity), maxEntities, poolBuffer);

    return context;
}

void DestroyGamecontext(GameContext* context)
{
    Pool_Clear(&context->entityPool);
    context->entities = nullptr;
    context->entityCount = 0;
    context->maxEntities = 0;
}

Entity* SpawnEntity(GameContext* context, EntityType type, v3 position)
{
    if (context->entityCount >= context->maxEntities)
    {
        // Handle error, maximum number of entities reached
        return nullptr;
    }

    Entity* entity = (Entity*)Pool_Alloc(&context->entityPool);
    entity->type = type;
    entity->transform.enabled = true;
    entity->transform.position = position;
    entity->transform.scale = v3(1.0f);
    entity->tileX = (u16)position.x;
    entity->tileY = (u16)position.y;
    context->entities[context->entityCount] = entity;
    context->entityCount++;

    Level_AddEntity(context->level, entity);

    return entity;
}

void DestroyEntity(GameContext* context, Entity* entity)
{
    for (u32 i = 0; i < context->entityCount; ++i)
    {
        if (context->entities[i] == entity)
        {
            Pool_Free(&context->entityPool, entity);
            context->entities[i] = context->entities[context->entityCount - 1];
            context->entityCount--;
            break;
        }
    }
}

void InitGamecontext(GameContext* context, u32 maxEntities, Arena* arena)
{
    Pool_Init(&context->entityPool, sizeof(Entity), maxEntities, Arena_PushSize(arena, maxEntities * sizeof(Entity)));
    context->entities = (Entity**)Arena_PushSize(arena, maxEntities * sizeof(Entity*));
    context->entityCount = 0;
}

void ShutdownGamecontext(GameContext* context)
{
    Pool_Clear(&context->entityPool);
    context->entityCount = 0;
}

void Game_Init(void* userBuffer, u64 userBufferSize)
{
    game = (Game*)userBuffer;
    ClearMemory(game, sizeof(Game));
    userBufferSize -= sizeof(Game);

    // Use 80% of the user buffer for the permanent arena and 20% for the transient arena.
    u64 permanentArenaSize = (u64)(userBufferSize * 0.8);
    u64 transientArenaSize = (u64)(userBufferSize * 0.2);
    void* permanentArenaBuffer = (u8*)userBuffer + sizeof(Game);
    void* transientArenaBuffer = (u8*)userBuffer + sizeof(Game) + permanentArenaSize;
    Arena_Init(&game->permanentArena, permanentArenaSize, permanentArenaBuffer);
    Arena_Init(&game->transientArena, transientArenaSize, transientArenaBuffer);

    game->settings.mouseSensitivity = 0.1f;

    // Initialize the game context with a grid of 24x16 and a maximum of 128 entities.
    game->context = CreateGamecontext(&game->permanentArena, 24, 16, 128);

    // Load audio clips
    sfxDoorOpen = Asset_LoadAudio("audio/door_open.wav");
    sfxPickupItem = Asset_LoadAudio("audio/pickup_item.wav");
}

void Game_Shutdown()
{
    if (game)
    {
        DestroyGamecontext(game->context);
        Arena_Clear(&game->permanentArena);
        Arena_Clear(&game->transientArena);
    }
}



void Game_Update(f32 deltaTime)
{
    GameContext* context = game->context;
    Level* level = context->level;

    for (u32 i = 0; i < context->entityCount; ++i)
    {
        Entity* entity = context->entities[i];

        switch (entity->type)
        {
            case EntityType_Player: {
                UpdatePlayer(context, entity, deltaTime);
            } break;

            case EntityType_Pickup: {
                UpdatePickup(context, entity, deltaTime);
            } break;

            case EntityType_Door: {
                UpdateDoor(context, entity, deltaTime);
            } break;
        }
    }

    AnimationSystem(context->entities, context->entityCount, deltaTime);
    MovementSystem(context->entities, context->entityCount, level, deltaTime);
    CollisionSystem(context->entities, context->entityCount, deltaTime);
}

void Game_Render()
{
    RenderSystem(game->context->entities, game->context->entityCount);
}

