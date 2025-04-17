#include "game.h"
#include "core/time.h"
#include "platform/platform.h"
#include "render/render.h"
#include "render/image.h"

void Trigger_OpenDoor(Level* level, Entity* entity)
{
    Tile* tile = LevelGetTile(level, entity->tile_x, entity->tile_y);
    Door* door = &level->doors[tile->door_index];
    DoorOpen(door);
}

void Trigger_CloseDoor(Level* level, Entity* entity)
{
    Tile* tile = LevelGetTile(level, entity->tile_x, entity->tile_y);
    Door* door = &level->doors[tile->door_index];
    DoorClose(door, level);
}

void Trigger_StayDoor(Level* level, Entity* entity)
{
    
}

void GameInit(Game* game)
{
    PlatformInit();

    // Game initialization
    u64 permanent_arena_size = 1024 * 1024 * 12;
    ArenaInit(&game->permanent_arena, permanent_arena_size, PlatformAllocateMemory(permanent_arena_size));
    InputInit();
    TimeInit(&game->time);
    RendererInit(&game->permanent_arena, 800, 600);

    game->state.camera = CameraCreateOrthographic(0.0f, 800.0f / 64.0f, 600.0f / 64.0f, 0.0f, -1.0f, 1.0f);

    // Game state initialization
    Image wall_image = ImageLoad("maps/test.bmp");
    u32* pixels = (u32*)wall_image.pixels;

    LevelInit(&game->state.level, wall_image.width, wall_image.height, &game->permanent_arena);

    game->state.player = SpawnEntity(&game->state.level.entity_manager);
    game->state.player->type = ENTITY_PLAYER;
    game->state.player->transform.angle = 0.0f;
    game->state.player->collider.radius = 0.3f;
    game->state.player->movement.speed = 32.0f;
    game->state.player->movement.friction = 0.85f;

    for (s32 y = 0; y < wall_image.height; ++y)
    {
        for (s32 x = 0; x < wall_image.width; ++x)
        {
            u32 pixel = pixels[y * wall_image.width + x];

            if (pixel == 0xff000000)
            {
                LevelSetTile(&game->state.level, x, y, TILE_WALL);
            }
            else if (pixel == 0xff00ff00)
            {
                LevelSetTile(&game->state.level, x, y, TILE_DOOR);
            }
            else if (pixel == 0xffff0000)
            {
                game->state.player->transform.position.x = x + 0.5f;
                game->state.player->transform.position.y = y + 0.5f;
            }
        }
    }

    Tile* tile = LevelGetTile(&game->state.level, 1, 3);
    tile->flags |= TF_TRIGGER;
    tile->on_enter = Trigger_OpenDoor;
    tile->on_exit = Trigger_CloseDoor;
    tile->on_stay = Trigger_StayDoor;
    tile->state.trigger.tile_x = 2;
    tile->state.trigger.tile_y = 4;

    ImageFree(&wall_image);

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

b32 EntityTryMove(Entity* entity, Level* level)
{
    Transform* transform = &entity->transform;
    Collider* collider = &entity->collider;

    Box2 box = EntityGetAABB(entity);

    s32 xmin = (s32)box.min.x;
    s32 ymin = (s32)box.min.y;
    s32 xmax = (s32)box.max.x;
    s32 ymax = (s32)box.max.y;

    for (s32 y = ymin; y <= ymax; ++y)
    {
        for (s32 x = xmin; x <= xmax; ++x)
        {
            if (IsSolid(level, x, y))
            {
                return false;
            }
        }
    }
    return true;
}

void EntityClipMoveVel(Entity* entity, Level* level)
{
    Transform* transform = &entity->transform;
    Movement* movement = &entity->movement;

    Vec2 position = transform->position;
    Vec2 velocity = movement->velocity;

    transform->last_position = position;
    transform->position = Vec2Add(position, velocity);

    if (EntityTryMove(entity, level))
    {
        return;
    }

    if (velocity.x != 0.0f)
    {
        transform->position.x = position.x + velocity.x;
        transform->position.y = position.y;
        movement->velocity.x = velocity.x;
        movement->velocity.y = 0.0f;
        if (EntityTryMove(entity, level))
        {
            return;
        }
    }

    if (velocity.y != 0.0f)
    {
        transform->position.x = position.x;
        transform->position.y = position.y + velocity.y;
        movement->velocity.x = 0.0f;
        movement->velocity.y = velocity.y;
        if (EntityTryMove(entity, level))
        {
            return;
        }
    }

    transform->position = position;
}

void GameFixedUpdate(Game* game, f32 dt)
{
    Entity* player = game->state.player;
    f32 speed = player->movement.speed;
    Vec2 forward = EntityGetForward(player);
    forward = Vec2Multiply(forward, speed * dt);

    Vec2 movement = {};

    if (game->input.key_down[KEY_UP])
    {
        player->movement.velocity = Vec2Add(player->movement.velocity, forward);
    }
    if (game->input.key_down[KEY_DOWN])
    {
        player->movement.velocity = Vec2Subtract(player->movement.velocity, forward);
    }

    //EntityClipMoveVel(player, &game->state.level);

    if (game->input.key_down[KEY_LEFT])
    {
        player->transform.angle -= 2.0f * dt;
    }
    if (game->input.key_down[KEY_RIGHT])
    {
        player->transform.angle += 2.0f * dt;
    }


    if (player->transform.angle > 2 * PI)
    {
        player->transform.angle -= 2 * PI;
    }
    if (player->transform.angle < 0.0f)
    {
        player->transform.angle += 2 * PI;
    }

    LevelUpdate(&game->state.level, dt);

}

void GameUpdate(Game* game, f32 dt)
{
    f64 now = PlatformGetTime();
    TimeUpdate(&game->time, now);
    InputUpdate(&game->input);

    // Center camera on player
    Camera* camera = &game->state.camera;
    camera->position.x = game->state.player->transform.position.x - camera->projection.orthographic.right * 0.5f;
    camera->position.y = game->state.player->transform.position.y - camera->projection.orthographic.bottom * 0.5f;

    // Clamp camera to level bounds
    if (camera->position.x < 0.0f)
    {
        camera->position.x = 0.0f;
    }
    if (camera->position.y < 0.0f)
    {
        camera->position.y = 0.0f;
    }
    if (camera->position.x > game->state.level.width - camera->projection.orthographic.right)
    {
        camera->position.x = game->state.level.width - camera->projection.orthographic.right;
    }
    if (camera->position.y > game->state.level.height - camera->projection.orthographic.bottom)
    {
        camera->position.y = game->state.level.height - camera->projection.orthographic.bottom;
    }


    Vec2 origin = game->state.player->transform.position;
    Vec2 direction = EntityGetForward(game->state.player);
    RayCastInfo ray_cast_info = LevelCastRay(&game->state.level, origin, direction);

    if (ray_cast_info.hit)
    {
        Tile* tile = ray_cast_info.tile;

        if (tile->type == TILE_DOOR && ray_cast_info.perp_distance < 0.5f && game->input.key_pressed[KEY_SPACE])
        {
            DoorOpen(&game->state.level.doors[tile->door_index]);
        }
    }
}

void GameRender(Game* game)
{
    RendererSetCamera(&game->state.camera);
    RendererBegin();

    for (u32 y = 0; y < game->state.level.height; ++y)
    {
        for (u32 x = 0; x < game->state.level.width; ++x)
        {
            Tile* tile = LevelGetTile(&game->state.level, x, y);
            Vec2 position = Vec2{ (f32)x, (f32)y };
            Vec2 size = Vec2{ 1.0f, 1.0f };
            if (tile->type == TILE_WALL)
            {
                RendererDrawFilleRect2D(position, size, COLOR_GRAY);
            }
            else if (tile->type == TILE_DOOR)
            {
                Door* door = &game->state.level.doors[tile->door_index];
                if (door->state == DoorState_Open)
                {
                    RendererDrawFilleRect2D(position, size, COLOR_GREEN);
                }
                else
                {
                    RendererDrawFilleRect2D(position, size, COLOR_RED);
                }
            }
            else
            {
                if (tile->flags & TF_TRIGGER)
                {
                    if (tile->state.trigger.active)
                    {
                        RendererDrawFilleRect2D(position, size, COLOR_GREEN);
                    }
                    else
                    {
                        RendererDrawFilleRect2D(position, size, COLOR_ORANGE);
                    }
                }
            }
        }
    }

    // Draw grid
    for (u32 y = 0; y < game->state.level.height; ++y)
    {
        Vec2 line_start = Vec2{ 0.0f, (f32)y };
        Vec2 line_end = Vec2{ (f32)game->state.level.width, (f32)y };
        RendererDrawLine2D(line_start, line_end, COLOR_DARK_GRAY);
    }

    for (u32 x = 0; x < game->state.level.width; ++x)
    {
        Vec2 line_start = Vec2{ (f32)x, 0.0f };
        Vec2 line_end = Vec2{ (f32)x, (f32)game->state.level.height };
        RendererDrawLine2D(line_start, line_end, COLOR_DARK_GRAY);
    }

    DrawEntities(&game->state.level.entity_manager);
    
    Vec2 origin = game->state.player->transform.position;
    Vec2 direction = EntityGetForward(game->state.player);
    RayCastInfo ray_cast_info = LevelCastRay(&game->state.level, origin, direction);

    if (ray_cast_info.hit)
    {
        RendererDrawLine2D(origin, ray_cast_info.wall_hit, COLOR_RED);
        RendererDrawCircle2D(ray_cast_info.wall_hit, 0.1f, COLOR_YELLOW);
    }


    RendererEnd();
}