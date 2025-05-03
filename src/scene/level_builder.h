#pragma once
#include "core/types.h"
#include "scene/level.h"
#include "renderer/image.h"
#include <glm/glm.hpp>

struct Surface
{
    glm::vec3 vertices[4];
    glm::vec2 texcoords[4];
    glm::vec2 lightmap[4];
    glm::vec3 normal;
};

struct Atlas
{
    s32 padding;
    s32 tileWidth;
    s32 tileHeight;
    s32 tilesPerRow;
    s32 tilesPerCol;
    Image image;
};

struct Light
{
    glm::vec3 position;
    glm::vec3 color;
    f32 intensity;
    f32 radius;
};

// Creates and initializes an atlas.
Atlas CreateAtlas(s32 width, s32 height, s32 tileWidth, s32 tileHeight, s32 padding, Arena* arena);

// Returns the min coordinates of the tile in the atlas without padding.
glm::ivec2 Atlas_GetTileMinAt(const Atlas* atlas, s32 tileId);

// Initializes the atlas tiles from surfaces.
void ComputeLightmap(Atlas* atlas, const Level* level, const Surface* surface, s32 surfaceCount, const Light* lights, s32 lightCount);

// Creates a surface for each tile in the level
Surface* CreateLevelSurfaces(const Level* level, Arena* transientStorage, s32* outSurfaceCount);

// Compute the lightmap texture cooridantes for each surface
void ComputeLightmapCoordinates(Surface* surfaces, s32 surfaceCount, const Level* level, const Atlas* atlas);