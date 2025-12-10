#pragma once
#include "core/types.h"
#include <glm/glm.hpp>

struct MapVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec2 lightmap;
};

struct MapSurface
{
    u32 vertices[4];
    u32 textureId;
    u32 lightmapId;
};

struct MapLight
{
    glm::vec3 position;
    glm::vec3 color;
    f32 intensity;
    f32 radius;
};

struct MapData
{
    MapVertex* vertices;
    u32 vertexCount;

    MapSurface* surfaces;
    u32 surfaceCount;

    MapLight* lights;
    u32 lightCount;
};