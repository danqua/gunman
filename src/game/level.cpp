#include "level.h"
#include "platform/platform.h"

static void HandleLevelCollision(Level* level)
{
    Entity** entities = level->entity_manager.entities;
    u64 entity_count = level->entity_manager.entity_count;

    for (u64 i = 0; i < entity_count; ++i)
    {
        Entity* entity = entities[i];
        Vec2* position = &entity->transform.position;
        Vec2* velocity = &entity->movement.velocity;

        s32 xmin = (s32)(position->x - entity->collider.radius);
        s32 ymin = (s32)(position->y - entity->collider.radius);
        s32 xmax = (s32)(position->x + entity->collider.radius);
        s32 ymax = (s32)(position->y + entity->collider.radius);

        for (s32 y = ymin; y <= ymax; ++y)
        {
            for (s32 x = xmin; x <= xmax; ++x)
            {
                if (IsSolid(level, x, y))
                {
                    Box2 entity_box = EntityGetAABB(entity);
                    Vec2 entity_center = Box2Center(entity_box);

                    Box2 tile_box;
                    tile_box.min = Vec2{ (f32)x, (f32)y };
                    tile_box.max = Vec2Add(tile_box.min, Vec2{ 1.0f, 1.0f });

                    Vec2 tile_center = Box2Center(tile_box);

                    Vec2 entity_tile = Vec2Subtract(tile_center, entity_center);

                    // The entity is either on the left or the right side
                    if (Abs(entity_tile.x) > Abs(entity_tile.y))
                    {
                        // The entity is on the left side
                        if (entity_tile.x >= 0.0f)
                        {
                            // Clip the entity to the box
                            position->x = tile_box.min.x - entity->collider.radius;
                        }
                        // The entity is on the right side
                        else
                        {
                            // Clip the entity to the box
                            position->x = tile_box.max.x + entity->collider.radius;
                        }
                        velocity->x = 0.0f;
                    }
                    // The entity is either on the top or the bottom side
                    else
                    {
                        // The entity is on the top side
                        if (entity_tile.y >= 0.0f)
                        {
                            // Clip the entity to the box
                            position->y = tile_box.min.y - entity->collider.radius;
                        }
                        // The entity is on the bottom side
                        else
                        {
                            // Clip the entity to the box
                            position->y = tile_box.max.y + entity->collider.radius;
                        }
                        velocity->y = 0.0f;
                    }
                }
            }
        }
    }
}

void LevelInit(Level* level, u32 width, u32 height, Arena* arena)
{
    EntityManagerInit(&level->entity_manager, arena);

    u64 arena_size = sizeof(u32) * width * height + width * height * sizeof(Tile);
    ArenaInit(&level->arena, arena_size, ArenaPushSize(arena, arena_size));

    level->width = width;
    level->height = height;
    level->tiles = (Tile*)ArenaPushSize(&level->arena, width * height * sizeof(Tile));
}

void LevelFree(Level* level)
{
    ArenaReset(&level->arena);
}

void LevelUpdate(Level* level, f32 dt)
{
    UpdateEntities(&level->entity_manager, dt);
    HandleLevelCollision(level);
    
    u64 entity_count = level->entity_manager.entity_count;
    Entity** entities = level->entity_manager.entities;

    for (u64 i = 0; i < entity_count; ++i)
    {
        Entity* entity = entities[i];
        Vec2 last_position = entity->transform.last_position;
        Vec2 current_position = entity->transform.position;

        Tile* last_tile = LevelGetTile(level, entity->last_tile_x, entity->last_tile_y);
        Tile* current_tile = LevelGetTile(level, entity->tile_x, entity->tile_y);

        if (last_tile == current_tile)
        {
            if (current_tile->state.trigger.active && current_tile->on_stay)
            {
                current_tile->on_stay(level, entity);
            }
            continue;
        }

        if (last_tile && last_tile->on_exit)
        {
            last_tile->on_exit(level, entity);
            last_tile->state.trigger.active = false;
            last_tile->state.trigger.entity = nullptr;
        }

        if (current_tile)
        {
            if (current_tile->on_enter && !current_tile->state.trigger.active)
            {
                current_tile->on_enter(level, entity);
                current_tile->state.trigger.active = true;
                current_tile->state.trigger.entity = entity;
            }
        }
    }

    // Update doors
    for (u32 i = 0; i < level->doors_count; ++i)
    {
        Door* door = &level->doors[i];
        DoorUpdate(door, level, dt);
    }
}

void LevelSetTile(Level* level, u32 x, u32 y, u32 tile_type)
{
    u32 index = y * level->width + x;
    level->tiles[index].type = tile_type;

    switch (tile_type)
    {
        case TILE_WALL: {
            Tile* tile = &level->tiles[index];
            tile->x = x;
            tile->y = y;
            tile->type = tile_type;
        } break;

        case TILE_DOOR: {
            Tile* tile = &level->tiles[index];
            tile->x = x;
            tile->y = y;
            tile->type = tile_type;
            tile->door_index = level->doors_count;
            Door* door = &level->doors[level->doors_count++];
            door->tile_x = x;
            door->tile_y = y;
        } break;
    }
}

Tile* LevelGetTile(Level* level, u32 x, u32 y)
{
    u32 index = y * level->width + x;
    return &level->tiles[index];
}

Entity* LevelGetEntityAt(Level* level, u32 x, u32 y)
{
    for (u64 i = 0; i < level->entity_manager.entity_count; ++i)
    {
        Entity* entity = level->entity_manager.entities[i];

        if (entity->tile_x == x && entity->tile_y == y)
        {
            return entity;
        }
    }
    return nullptr;
}

b32 LevelEntityCanMoveTo(Level* level, Entity* entity, f32 x, f32 y)
{
    f32 left = x - entity->collider.radius;
    f32 right = x + entity->collider.radius;
    f32 top = y - entity->collider.radius;
    f32 bottom = y + entity->collider.radius;

    u32 left_tile = (u32)left;
    u32 right_tile = (u32)right;
    u32 top_tile = (u32)top;
    u32 bottom_tile = (u32)bottom;

    if (left_tile < 0 || right_tile >= level->width || top_tile < 0 || bottom_tile >= level->height)
    {
        return false;
    }

    for (u32 y = top_tile; y <= bottom_tile; ++y)
    {
        for (u32 x = left_tile; x <= right_tile; ++x)
        {
            Tile* tile = LevelGetTile(level, x, y);
            if (tile->type == TILE_WALL)
            {
                return false;
            }
            else if (tile->type == TILE_DOOR)
            {
                Door* door = &level->doors[tile->door_index];
                if (door->state != DoorState_Open)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

b32 IsSolid(Level* level, u32 x, u32 y)
{
    Tile* tile = LevelGetTile(level, x, y);

    if (tile->type == TILE_DOOR)
    {
        Door* door = &level->doors[tile->door_index];
        return (door->state != DoorState_Open);
    }

    return tile->type == TILE_WALL;
}

b32 IsTrigger(Level* level, u32 x, u32 y)
{
    Tile* tile = LevelGetTile(level, x, y);
    return (tile->flags & TF_TRIGGER) != 0;
}

RayCastInfo LevelCastRay(Level* level, Vec2 origin, Vec2 direction, u32 max_depth)
{
    Point2D level_position = {
        (s32)origin.x,
        (s32)origin.y
    };

    Vec2 delta_dist = {
        (direction.x != 0.0f) ? Abs(1.0f / direction.x) : 1e30f,
        (direction.y != 0.0f) ? Abs(1.0f / direction.y) : 1e30f
    };

    Vec2 step = {
        direction.x > 0.0f ? 1.0f : -1.0f,
        direction.y > 0.0f ? 1.0f : -1.0f
    };

    Vec2 side_dist = {
        (direction.x > 0.0f) ? (level_position.x + 1.0f - origin.x) * delta_dist.x : (origin.x - level_position.x) * delta_dist.x,
        (direction.y > 0.0f) ? (level_position.y + 1.0f - origin.y) * delta_dist.y : (origin.y - level_position.y) * delta_dist.y
    };


    b32 hit = false;
    b32 side = false;
    u32 depth = 0;

    while (!hit && depth < max_depth)
    {
        if (side_dist.x < side_dist.y)
        {
            side_dist.x += delta_dist.x;
            level_position.x += (s32)step.x;
            side = false;
        }
        else
        {
            side_dist.y += delta_dist.y;
            level_position.y += (s32)step.y;
            side = true;
        }

        Tile* tile = LevelGetTile(level, level_position.x, level_position.y);

        if (IsSolid(level, level_position.x, level_position.y))
        {
            hit = true;
        }
        depth++;
    }

    f32 perp_dist = side ? side_dist.y - delta_dist.y : side_dist.x - delta_dist.x;
    
    Vec2 wall_hit = {
        origin.x + direction.x * perp_dist,
        origin.y + direction.y * perp_dist
    };

    RayCastInfo result = {};
    result.hit = hit;
    result.wall_hit = wall_hit;
    result.tile = LevelGetTile(level, level_position.x, level_position.y);
    result.perp_distance = perp_dist;
    return result;
}

static void DrawGrid2D(Level* level, Color color)
{
    for (u32 y = 0; y < level->height; ++y)
    {
        Vec2 line_start = Vec2{ 0.0f, (f32)y };
        Vec2 line_end = Vec2{ (f32)level->width, (f32)y };
        RendererDrawLine2D(line_start, line_end, COLOR_DARK_GRAY);
    }

    for (u32 x = 0; x < level->width; ++x)
    {
        Vec2 line_start = Vec2{ (f32)x, 0.0f };
        Vec2 line_end = Vec2{ (f32)x, (f32)level->height };
        RendererDrawLine2D(line_start, line_end, COLOR_DARK_GRAY);
    }
}

static void DrawWalls2D(Level* level)
{
    for (u32 y = 0; y < level->height; ++y)
    {
        for (u32 x = 0; x < level->width; ++x)
        {
            Tile* tile = LevelGetTile(level, x, y);
            Vec2 position = Vec2{ (f32)x, (f32)y };
            Vec2 size = Vec2{ 1.0f, 1.0f };
            if (tile->type == TILE_WALL)
            {
                RendererDrawFilleRect2D(position, size, COLOR_GRAY);
            }
            else if (tile->type == TILE_DOOR)
            {
                Door* door = &level->doors[tile->door_index];
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
}

void LevelDraw2D(Level* level, Camera* camera)
{
    RendererSetCamera(camera);

    if (camera->type == CAMERA_TYPE_ORTHOGRAPHIC)
    {
        DrawWalls2D(level);
        DrawGrid2D(level, COLOR_DARK_GRAY);
        DrawEntities(&level->entity_manager);
    }
}