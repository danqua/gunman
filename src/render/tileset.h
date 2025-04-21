#pragma once
#include "render/texture.h"
#include "core/math.h"

struct Tileset
{
    s32 tile_width;
    s32 tile_height;
    Texture* texture;
};

// Loads a texture from file and creates a tileset.
Tileset LoadTileset(const char* path, s32 tile_width, s32 tile_height);

// Frees the texture of the tileset.
void UnloadTileset(Tileset* tileset);

// Returns the uv coordinates of a tile in the tileset.
void GetTileTexCoords(Tileset* tileset, u32 tile_id, Vec2* uv1, Vec2* uv2, Vec2* uv3, Vec2* uv4);