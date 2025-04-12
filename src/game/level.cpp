#include "level.h"

void LevelInit(Level* level, u32 width, u32 height, Arena* arena)
{
    EntityManagerInit(&level->entity_manager, arena);

    u64 arena_size = sizeof(u32) * width * height + width * height * sizeof(Tile);
    ArenaInit(&level->arena, arena_size, ArenaPushSize(arena, arena_size));

    level->width = width;
    level->height = height;
    level->tiles = (u32*)ArenaPushSize(&level->arena, width * height * sizeof(u32));
    level->tile_data = (Tile*)ArenaPushSize(&level->arena, width * height * sizeof(Tile));
}

void LevelFree(Level* level)
{
    ArenaReset(&level->arena);
}

void LevelSetTile(Level* level, u32 x, u32 y, u32 tile_type)
{
    u32 index = y * level->width + x;
    level->tiles[index] = tile_type;

    Tile* tile = &level->tile_data[index];
    tile->x = x;
    tile->y = y;
    tile->type = tile_type;

    switch (tile_type)
    {
        case TILE_WALL: {
            
        } break;
    }
}

u32 LevelGetTile(Level* level, u32 x, u32 y)
{
    u32 index = y * level->width + x;
    return level->tiles[index];
}

b32 LevelCanMoveTo(Level* level, Entity* entity, f32 x, f32 y)
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
            u32 tile = LevelGetTile(level, x, y);
            if (tile != 0)
            {
                return false;
            }
        }
    }

    return true;
}

u64 LevelGetTiles(Level* level, Vec2 min, Vec2 max, u32* tiles, u64 tile_count)
{
    s32 min_x = (s32)Min(min.x, max.x);
    s32 max_x = (s32)Max(min.x, max.x);
    s32 min_y = (s32)Min(min.y, max.y);
    s32 max_y = (s32)Max(min.y, max.y);

    u64 tile_index = 0;

    for (s32 y = min_y; y <= max_y; ++y)
    {
        for (s32 x = min_x; x <= max_x; ++x)
        {
            u32 tile = LevelGetTile(level, x, y);

            if (tile != 0)
            {
                tiles[tile_index] = y * level->width + x;
                tile_index++;

                if (tile_index >= tile_count)
                {
                    return tile_count;
                }
            }
        }
    }
    return tile_index;
}

b32 IsSolid(Level* level, u32 x, u32 y)
{
    u32 tile = LevelGetTile(level, x, y);
    return tile == 1;
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

        u32 tile = LevelGetTile(level, level_position.x, level_position.y);

        if (tile != 0)
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
    return result;
}