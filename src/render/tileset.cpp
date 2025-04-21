#include "tileset.h"

Tileset LoadTileset(const char* path, s32 tile_width, s32 tile_height)
{
    Tileset result = {};
    result.tile_width = tile_width;
    result.tile_height = tile_height;
    result.texture = TextureLoad(path);
    if (!result.texture)
    {
        return Tileset{};
    }

    return result;
}

void UnloadTileset(Tileset* tileset)
{
    if (tileset->texture)
    {
        TextureDestroy(tileset->texture);
        tileset->texture = nullptr;
    }
    tileset->tile_width = 0;
    tileset->tile_height = 0;
}

void GetTileTexCoords(Tileset* tileset, u32 tile_id, Vec2* uv1, Vec2* uv2, Vec2* uv3, Vec2* uv4)
{
    s32 tiles_per_row = TextureGetWidth(tileset->texture) / tileset->tile_width;
    s32 row = tile_id / tiles_per_row;
    s32 col = tile_id % tiles_per_row;

    f32 u = (f32)col * tileset->tile_width;
    f32 v = (f32)row * tileset->tile_height;

    uv1->x = u / TextureGetWidth(tileset->texture);
    uv1->y = (v + tileset->tile_height) / TextureGetHeight(tileset->texture);

    uv2->x = (u + tileset->tile_width) / TextureGetWidth(tileset->texture);
    uv2->y = (v + tileset->tile_height) / TextureGetHeight(tileset->texture);

    uv3->x = (u + tileset->tile_width) / TextureGetWidth(tileset->texture);
    uv3->y = v / TextureGetHeight(tileset->texture);

    uv4->x = u / TextureGetWidth(tileset->texture);
    uv4->y = v / TextureGetHeight(tileset->texture);
}