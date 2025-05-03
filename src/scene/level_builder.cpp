#include "level_builder.h"
#include "renderer/color.h"
#include "core/platform.h"

static void CreateFloorVertices(Surface* surface, glm::vec2 position)
{
    surface->vertices[0] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 0.0f);
    surface->vertices[1] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 1.0f);
    surface->vertices[2] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 1.0f);
    surface->vertices[3] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 0.0f);

    glm::vec3 v1 = surface->vertices[1] - surface->vertices[0];
    glm::vec3 v2 = surface->vertices[2] - surface->vertices[0];
    surface->normal = glm::normalize(glm::cross(v1, v2));

    surface->texcoords[0] = glm::vec2(0.0f, 0.0f);
    surface->texcoords[1] = glm::vec2(0.0f, 1.0f);
    surface->texcoords[2] = glm::vec2(1.0f, 1.0f);
    surface->texcoords[3] = glm::vec2(1.0f, 0.0f);
}

static void CreateCeilingVertices(Surface* surface, glm::vec2 position)
{
    surface->vertices[0] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 0.0f);
    surface->vertices[1] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 0.0f);
    surface->vertices[2] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 1.0f);
    surface->vertices[3] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 1.0f);

    glm::vec3 v1 = surface->vertices[1] - surface->vertices[0];
    glm::vec3 v2 = surface->vertices[2] - surface->vertices[0];
    surface->normal = glm::normalize(glm::cross(v1, v2));

    surface->texcoords[0] = glm::vec2(0.0f, 1.0f);
    surface->texcoords[1] = glm::vec2(1.0f, 1.0f);
    surface->texcoords[2] = glm::vec2(1.0f, 0.0f);
    surface->texcoords[3] = glm::vec2(0.0f, 0.0f);
}

static void CreateWallVertices(Surface* surface, WallDirection direction, glm::vec2 position)
{
    switch (direction)
    {
    case WallDirection_North: {
        surface->vertices[0] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 1.0f);
        surface->vertices[1] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 1.0f);
        surface->vertices[2] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 1.0f);
        surface->vertices[3] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 1.0f);
    } break;

    case WallDirection_South: {
        surface->vertices[0] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 0.0f);
        surface->vertices[1] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 0.0f);
        surface->vertices[2] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 0.0f);
        surface->vertices[3] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 0.0f);
    } break;

    case WallDirection_East: {
        surface->vertices[0] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 1.0f);
        surface->vertices[1] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 0.0f);
        surface->vertices[2] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 0.0f);
        surface->vertices[3] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 1.0f);
    } break;

    case WallDirection_West: {
        surface->vertices[0] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 0.0f);
        surface->vertices[1] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 1.0f);
        surface->vertices[2] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 1.0f);
        surface->vertices[3] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 0.0f);
    } break;
    };

    glm::vec3 v1 = surface->vertices[1] - surface->vertices[0];
    glm::vec3 v2 = surface->vertices[2] - surface->vertices[0];
    surface->normal = glm::normalize(glm::cross(v1, v2));

    surface->texcoords[0] = glm::vec2(0.0f, 0.0f);
    surface->texcoords[1] = glm::vec2(1.0f, 0.0f);
    surface->texcoords[2] = glm::vec2(1.0f, 1.0f);
    surface->texcoords[3] = glm::vec2(0.0f, 1.0f);
}

Surface* CreateLevelSurfaces(const Level* level, Arena* transientStorage, s32* outSurfaceCount)
{
    *outSurfaceCount = LevelGetSurfaceCount(level);
    Surface *surfaces = (Surface*)Arena_PushSize(transientStorage, sizeof(Surface) * (*outSurfaceCount));
    s32 surfaceIndex = 0;

    for (s32 y = 0; y < level->height; ++y)
    {
        for (s32 x = 0; x < level->width; ++x)
        {
            Tile* tile = LevelGetTileAt(level, x, y);
            if (tile && tile->type == TileType_Empty)
            {
                Surface surface = {};
                CreateFloorVertices(&surface, glm::vec2(x, y));
                surfaces[surfaceIndex++] = surface;

                CreateCeilingVertices(&surface, glm::vec2(x, y));
                surfaces[surfaceIndex++] = surface;
                continue;
            }

            Tile* north = LevelGetTileAt(level, x, y + 1);
            if (north && north->type == TileType_Empty)
            {
                Surface surface = {};
                CreateWallVertices(&surface, WallDirection_North, glm::vec2(x, y));
                surfaces[surfaceIndex++] = surface;
            }

            Tile* south = LevelGetTileAt(level, x, y - 1);
            if (south && south->type == TileType_Empty)
            {
                Surface surface = {};
                CreateWallVertices(&surface, WallDirection_South, glm::vec2(x, y));
                surfaces[surfaceIndex++] = surface;
            }

            Tile* east = LevelGetTileAt(level, x + 1, y);
            if (east && east->type == TileType_Empty)
            {
                Surface surface = {};
                CreateWallVertices(&surface, WallDirection_East, glm::vec2(x, y));
                surfaces[surfaceIndex++] = surface;
            }

            Tile* west = LevelGetTileAt(level, x - 1, y);
            if (west && west->type == TileType_Empty)
            {
                Surface surface = {};
                CreateWallVertices(&surface, WallDirection_West, glm::vec2(x, y));
                surfaces[surfaceIndex++] = surface;
            }
        }
    }

    return surfaces;
}

Atlas CreateAtlas(s32 width, s32 height, s32 tileWidth, s32 tileHeight, s32 padding, Arena* arena)
{
    Atlas atlas = {};
    atlas.image.width = width;
    atlas.image.height = height;
    atlas.image.pixels = Arena_PushSize(arena, width * height * sizeof(u32));

    atlas.padding = padding;
    atlas.tileWidth = tileWidth;
    atlas.tileHeight = tileHeight;
    atlas.tilesPerRow = width / (tileWidth + padding * 2);
    atlas.tilesPerCol = height / (tileHeight + padding * 2);

    return atlas;
}

glm::ivec2 Atlas_GetTileMinAt(const Atlas* atlas, s32 tileId)
{
    s32 atlasCol = tileId % atlas->tilesPerRow;
    s32 atlasRow = tileId / atlas->tilesPerRow;
    s32 tilePaddedWidth = atlas->tileWidth + atlas->padding * 2;
    s32 tilePaddedHeight = atlas->tileHeight + atlas->padding * 2;

    glm::ivec2 result = {};
    result.x = atlasCol * tilePaddedWidth;
    result.y = atlasRow * tilePaddedHeight;
    return result;
}

void ComputeLightmap(Atlas* atlas, const Level* level, const Surface* surfaces, s32 surfaceCount, const Light* lights, s32 lightCount)
{
    Image_FillColor(&atlas->image, Color{ 0, 0, 0, 255 });

    Image tImage = {};
    tImage.width = atlas->tileWidth;
    tImage.height = atlas->tileHeight;
    tImage.pixels = Platform_Alloc(sizeof(u32) * atlas->tileWidth * atlas->tileHeight);

    for (s32 i = 0; i < lightCount; ++i)
    {
        const Light* light = &lights[i];

        for (s32 j = 0; j < surfaceCount; ++j)
        {
            const Surface* surface = &surfaces[j];

            glm::vec3 uAxis = glm::normalize(surface->vertices[1] - surface->vertices[0]);
            glm::vec3 vAxis = glm::normalize(surface->vertices[3] - surface->vertices[0]);
                        
            f32 luxelWidth = (f32)atlas->tileWidth / (f32)atlas->image.width;

            Image_FillColor(&tImage, Color{ 0, 0, 0, 255 });
            
            for (s32 y = 0; y < atlas->tileHeight; ++y)
            {
                for (s32 x = 0; x < atlas->tileWidth; ++x)
                {
                    f32 uOffset = (x + 0.5f) * (1.0f / atlas->tileWidth);
                    f32 vOffset = (y + 0.5f) * (1.0f / atlas->tileHeight);
                    glm::vec3 luxelPosition = surface->vertices[0] + uAxis * uOffset + vAxis * vOffset;
                    glm::vec3 lightRay = glm::normalize(luxelPosition - light->position);

                    f32 bias = 0.01f;
                    glm::vec2 direction = glm::normalize(glm::vec2(lightRay.x, lightRay.z));
                    glm::vec2 origin = glm::vec2(light->position.x, light->position.z) + direction * bias;
                    f32 luxelDistance = glm::distance(glm::vec2(luxelPosition.x, luxelPosition.z), glm::vec2(light->position.x, light->position.z));

                    // Get the current color in the lightmap
                    glm::ivec2 tileMin = Atlas_GetTileMinAt(atlas, j);
                    tileMin.x += x + atlas->padding;
                    tileMin.y += y + atlas->padding;
                    u32 currentPixel = ((u32*)atlas->image.pixels)[tileMin.x + tileMin.y * atlas->image.width];
                    glm::vec4 currentColor = Color_ConvertToVec4(Color_ConvertToRGBA(currentPixel, PixelFormat_ABGR));
                    glm::vec4 fragColor = currentColor;


                    RayCastHit hit = {};
                    if (LevelCastRay(level, origin, direction, &hit))
                    {
                        if (hit.distance + 0.02f > luxelDistance)
                        {
                            f32 lConstant = 1.0f;
                            f32 lLinear = 0.35f;
                            f32 lQuadratic = 0.44f;
                            f32 attenuation = 1.0f / (lConstant + lLinear * luxelDistance + lQuadratic * (luxelDistance * luxelDistance));

                            fragColor += glm::vec4(light->color * light->intensity * attenuation, 1.0f);
                            fragColor = glm::clamp(fragColor, 0.0f, 1.0f);
                        }
                    }

                    u32* pixels = (u32*)tImage.pixels;
                    pixels[y * atlas->tileWidth + x] = Color_ConvertToU32(Color_ConvertToColor(fragColor), PixelFormat_ABGR);
                }
            }

            glm::ivec2 tileMin = Atlas_GetTileMinAt(atlas, j);
            Image_Blit(&atlas->image, &tImage, tileMin.x + atlas->padding, tileMin.y + atlas->padding);
        }
    }

    Platform_Free(tImage.pixels);
}

void ComputeLightmapCoordinates(Surface* surfaces, s32 surfaceCount, const Level* level, const Atlas* atlas)
{
    for (s32 i = 0; i < surfaceCount; ++i)
    {
        Surface* surface = &surfaces[i];
        glm::ivec2 tileMin = Atlas_GetTileMinAt(atlas, i);
        f32 u = ((f32)tileMin.x + atlas->padding) / (f32)atlas->image.width;
        f32 v = ((f32)tileMin.y + atlas->padding) / (f32)atlas->image.height;
        f32 lx = atlas->tileWidth / (f32)atlas->image.width;

        surface->lightmap[0] = glm::vec2(u     , v     );
        surface->lightmap[1] = glm::vec2(u + lx, v     );
        surface->lightmap[2] = glm::vec2(u + lx, v + lx);
        surface->lightmap[3] = glm::vec2(u     , v + lx);
    }
}