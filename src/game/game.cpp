#include "game.h"
#include "core/time.h"
#include "platform/platform.h"
#include "render/render.h"
#include "render/image.h"
#include "level_renderer.h"

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

    game->state.camera = CameraCreatePerspective(70.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    game->state.camera.position = Vec3{ 0.0f, 0.0f, 2.0f };

    game->state.camera_2d = CameraCreateOrthographic(0.0f, 1280.0f / 64.0f, 720.0f / 64.0f, 0.0f, -1.0f, 1.0f);

    // Game state initialization
    Image wall_image = ImageLoad("maps/test.bmp");
    u32* pixels = (u32*)wall_image.pixels;

    LevelInit(&game->state.level, wall_image.width, wall_image.height, &game->permanent_arena);

    game->state.tileset = LoadTileset("textures/tileset.bmp", 32, 32);

    game->state.player = SpawnEntity(&game->state.level.entity_manager);
    game->state.player->type = ENTITY_PLAYER;
    game->state.player->transform.angle = 0.0f;
    game->state.player->collider.radius = 0.3f;
    game->state.player->movement.speed = 32.0f;
    game->state.player->movement.friction = 0.85f;
    game->state.player_fov = 45.0f;

    for (s32 y = 0; y < wall_image.height; ++y)
    {
        for (s32 x = 0; x < wall_image.width; ++x)
        {
            u32 pixel = pixels[y * wall_image.width + x];

            if (pixel == 0xff000000 || pixel == 0xff008000)
            {
                LevelSetTile(&game->state.level, x, y, TILE_WALL);

                if (pixel == 0xff008000)
                {
                    LevelGetTile(&game->state.level, x, y)->flags = 1;
                }
                
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
    InputState* input = &game->input;
    Camera* camera = &game->state.camera;

    Entity* player = game->state.player;
    f32 speed = player->movement.speed;
    Vec2 forward = EntityGetForward(player);
    Vec2 right = Vec2{ forward.y, -forward.x };

    forward = Vec2Multiply(forward, speed * dt);
    right = Vec2Multiply(right, speed * dt);

    Vec2 movement = {};

    if (game->input.key_down[KEY_UP] || game->input.key_down[KEY_W])
    {
        player->movement.velocity = Vec2Add(player->movement.velocity, forward);
    }
    if (game->input.key_down[KEY_DOWN] || game->input.key_down[KEY_S])
    {
        player->movement.velocity = Vec2Subtract(player->movement.velocity, forward);
    }
    if (game->input.key_down[KEY_A])
    {
        player->movement.velocity = Vec2Add(player->movement.velocity, right);
    }
    if (game->input.key_down[KEY_D])
    {
        player->movement.velocity = Vec2Subtract(player->movement.velocity, right);
    }

    Vec2 origin = game->state.player->transform.position;
    Vec2 direction = EntityGetForward(game->state.player);
    RayCastInfo ray_cast_info = LevelCastRay(&game->state.level, origin, direction);

    if (ray_cast_info.hit)
    {
        Tile* tile = ray_cast_info.tile;

        if (tile->type == TILE_DOOR && ray_cast_info.perp_distance < 1.0f && (game->input.key_pressed[KEY_SPACE] || game->input.key_pressed[KEY_E]))
        {
            DoorOpen(&game->state.level.doors[tile->door_index]);
        }
    }

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
    Camera* camera = &game->state.camera_2d;
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


    game->state.camera.position.x = game->state.player->transform.position.x;
    game->state.camera.position.z = game->state.player->transform.position.y;
    game->state.camera.position.y = 0.5f;
    game->state.camera.rotation.y = RadToDeg(game->state.player->transform.angle) + 90.0f;


    f32 mouse_sensitivity = 48.0f;
    game->state.player->transform.angle += DegToRad(game->input.mouse_dx * mouse_sensitivity * dt);
    game->state.camera.rotation.x -= game->input.mouse_dy * mouse_sensitivity * dt;
}

void GameRender(Game* game)
{
    RendererBegin();

    RendererSetCamera(&game->state.camera);
    DrawLevel(&game->state.level, &game->state.tileset);

    if (game->input.key_down[KEY_TAB])
    {
        LevelDraw2D(&game->state.level, &game->state.tileset, &game->state.camera_2d);
    }

    RendererEnd();
}