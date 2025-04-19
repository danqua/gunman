#include "level_renderer.h"
#include "render/render.h"

enum WallDirection
{
    WallDirection_North,
    WallDirection_East,
    WallDirection_South,
    WallDirection_West
};

static void DrawWall(Vec2 position, WallDirection direction, Color color)
{
    Vec3 v1, v2, v3, v4;
    
    switch (direction)
    {
        case WallDirection_North: {
            v1 = Vec3{ position.x, 0.0f, position.y };
            v2 = Vec3{ position.x + 1.0f, 0.0f, position.y };
            v3 = Vec3{ position.x + 1.0f, 1.0f, position.y };
            v4 = Vec3{ position.x, 1.0f, position.y };
            RendererDrawQuad(v1, v2, v3, v4, color);
        } break;

        case WallDirection_East: {
            v1 = Vec3{ position.x + 1.0f, 0.0f, position.y };
            v2 = Vec3{ position.x + 1.0f, 0.0f, position.y + 1.0f };
            v3 = Vec3{ position.x + 1.0f, 1.0f, position.y + 1.0f };
            v4 = Vec3{ position.x + 1.0f, 1.0f, position.y };
            RendererDrawQuad(v1, v2, v3, v4, color);
        } break;

        case WallDirection_South: {
            v1 = Vec3{ position.x + 1.0f, 0.0f, position.y + 1.0f };
            v2 = Vec3{ position.x, 0.0f, position.y + 1.0f };
            v3 = Vec3{ position.x, 1.0f, position.y + 1.0f };
            v4 = Vec3{ position.x + 1.0f, 1.0f, position.y + 1.0f };
            RendererDrawQuad(v1, v2, v3, v4, color);
        } break;

        case WallDirection_West: {
            v1 = Vec3{ position.x, 0.0f, position.y + 1.0f };
            v2 = Vec3{ position.x, 0.0f, position.y };
            v3 = Vec3{ position.x, 1.0f, position.y };
            v4 = Vec3{ position.x, 1.0f, position.y + 1.0f };
            RendererDrawQuad(v1, v2, v3, v4, color);
        } break;
    }
}

static void DrawFloor(Vec2 position)
{
    Vec3 v1 = Vec3{ position.x, 0.0f, position.y };
    Vec3 v2 = Vec3{ position.x, 0.0f, position.y + 1.0f };
    Vec3 v3 = Vec3{ position.x + 1.0f, 0.0f, position.y + 1.0f };
    Vec3 v4 = Vec3{ position.x + 1.0f, 0.0f, position.y };
    RendererDrawQuad(v1, v2, v3, v4, COLOR_GREEN);
}

static void DrawCeiling(Vec2 position)
{
    Vec3 v1 = Vec3{ position.x, 1.0f, position.y };
    Vec3 v2 = Vec3{ position.x + 1.0f, 1.0f, position.y };
    Vec3 v3 = Vec3{ position.x + 1.0f, 1.0f, position.y + 1.0f };
    Vec3 v4 = Vec3{ position.x, 1.0f, position.y + 1.0f };
    RendererDrawQuad(v1, v2, v3, v4, COLOR_GREEN);
}

static void DrawDoor(Door* door, Vec2 position, DoorAxis axis)
{
    DrawCeiling(position);
    DrawFloor(position);

    switch (axis)
    {
        case DoorAxis_Horizontal: {
            Vec2 pos = position;
            f32 t = door->timer / kDoorTransitionTime;

            DrawWall(position, WallDirection_West, COLOR_RED);

            if (door->state == DoorState_Opening)
            {
                pos = Vec2Lerp(position, Vec2Add(position, Vec2{ 1.0f, 0.0f }), t);
            }
            else if (door->state == DoorState_Closing)
            {
                pos = Vec2Lerp(position, Vec2Add(position, Vec2{ 1.0f, 0.0f }), 1.0f - t);
            }
            else if (door->state == DoorState_Open)
            {
                DrawWall(position, WallDirection_East, COLOR_DARK_GRAY);
                break;
            }
            DrawWall(pos, WallDirection_North, COLOR_GRAY);
            DrawWall(pos, WallDirection_South, COLOR_GRAY);
            DrawWall(pos, WallDirection_West, COLOR_DARK_GRAY);
        } break;

        case DoorAxis_Vertical: {
            Vec2 pos = position;
            f32 t = door->timer / kDoorTransitionTime;

            DrawWall(position, WallDirection_South, COLOR_BLUE);

            if (door->state == DoorState_Opening)
            {
                pos = Vec2Lerp(position, Vec2Add(position, Vec2{ 0.0f, -1.0f }), t);
            }
            else if (door->state == DoorState_Closing)
            {
                pos = Vec2Lerp(position, Vec2Add(position, Vec2{ 0.0f, -1.0f }), 1.0f - t);
            }
            else if (door->state == DoorState_Open)
            {
                DrawWall(position, WallDirection_North, COLOR_DARK_GRAY);
                break;
            }
            DrawWall(pos, WallDirection_East, COLOR_GRAY);
            DrawWall(pos, WallDirection_West, COLOR_GRAY);
            DrawWall(pos, WallDirection_South, COLOR_DARK_GRAY);
        } break;
    }

}

void DrawLevel(Level* level)
{
    for (u32 y = 1; y < level->height - 1; ++y)
    {
        for (u32 x = 1; x < level->width - 1; ++x)
        {
            u32 index = y * level->width + x;
            Tile* tile = &level->tiles[index];

            Tile* tt = LevelGetTile(level, x, (y - 1));
            Tile* bb = LevelGetTile(level, x, (y + 1));
            Tile* ll = LevelGetTile(level, (x - 1), y);
            Tile* rr = LevelGetTile(level, (x + 1), y);
            Tile* cc = LevelGetTile(level, x, y);

            Vec2 position = Vec2{ (float)x, (float)y };

            if (cc && cc->type == TILE_NONE)
            {
                DrawFloor(position);
                DrawCeiling(position);
            }
            else
            {
                continue;
            }

            if (tt)
            {
                switch (tt->type)
                {
                    case TILE_WALL: {
                        DrawWall(position, WallDirection_North, COLOR_BLUE);
                    } break;

                    case TILE_DOOR: {
                        Door* door = &level->doors[tt->door_index];
                        DrawDoor(door, Vec2Subtract(position, Vec2{ 0.0f, 1.0f }), DoorAxis_Horizontal);
                    } break;
                }
            }

            if (rr)
            {
                switch (rr->type)
                {
                    case TILE_WALL: {
                        DrawWall(position, WallDirection_East, COLOR_RED);
                    } break;

                    case TILE_DOOR: {
                        Door* door = &level->doors[rr->door_index];
                        DrawDoor(door, Vec2Add(position, Vec2{ 1.0f, 0.0f }), DoorAxis_Vertical);
                    } break;
                }

            }

            if (bb && bb->type == TILE_WALL)
            {
                DrawWall(position, WallDirection_South, COLOR_BLUE);
            }

            if (ll && ll->type == TILE_WALL)
            {
                DrawWall(position, WallDirection_West, COLOR_RED);
            }
        }
    }
}