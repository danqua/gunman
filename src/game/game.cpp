#include "game.h"
#include "core/time.h"
#include "platform/platform.h"
#include "render/render.h"

void GameInit(Game* game)
{
    PlatformInit();

    // Game initialization
    u64 permanent_arena_size = 1024 * 1024 * 12;
    ArenaInit(&game->permanent_arena, permanent_arena_size, PlatformAllocateMemory(permanent_arena_size));
    InputInit();
    TimeInit(&game->time);
    RendererInit(&game->permanent_arena, 800, 600);

    game->state.camera = CameraCreateOrthographic(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    // Game state initialization
    EntityManagerInit(&game->state.entity_manager, &game->permanent_arena);

    game->state.player = SpawnEntity(&game->state.entity_manager);
    game->state.player->type = ENTITY_PLAYER;
    game->state.player->transform.position = Vec2{ 32.0f, 32.0f };
    game->state.player->transform.angle = 0.0f;
    game->state.player->collider.radius = 16.0f;
    game->state.player->movement.speed = 4.0f;
    game->state.player->movement.friction = 0.85f;
}

void GameRun(Game* game)
{
    GameInit(game);

    while (!PlatformShouldQuit())
    {
        PlatformPollEvents();

        // Run physics/game logic in fixed steps.
        while (TimeShouldStep(&game->time))
        {
            GameFixedUpdate(game, game->time.fixed_dt);
        }

        // Smooth stuff
        GameUpdate(game, game->time.delta_time);

        GameRender(game);

        PlatformSwapBuffers();
    }


    RendererShutdown();
    PlatformShutdown();
}

void GameFixedUpdate(Game* game, f32 dt)
{
    Entity* player = game->state.player;
    f32 speed = player->movement.speed;
    Vec2 forward = EntityGetForward(player);
    forward = Vec2Multiply(forward, speed);

    if (game->input.key_down[KEY_UP])
    {
        player->movement.velocity = Vec2Add(player->movement.velocity, forward);
    }
    if (game->input.key_down[KEY_DOWN])
    {
        player->movement.velocity = Vec2Subtract(player->movement.velocity, forward);
    }

    if (game->input.key_down[KEY_LEFT])
    {
        player->transform.angle -= 2.0f * dt;
    }
    if (game->input.key_down[KEY_RIGHT])
    {
        player->transform.angle += 2.0f * dt;
    }

}

void GameUpdate(Game* game, f32 dt)
{
    f64 now = PlatformGetTime();
    TimeUpdate(&game->time, now);
    InputUpdate(&game->input);

    UpdateEntities(&game->state.entity_manager, dt);
}

void GameRender(Game* game)
{
    RendererSetCamera(&game->state.camera);
    RendererBegin();
    DrawEntities(&game->state.entity_manager);
    RendererEnd();
}