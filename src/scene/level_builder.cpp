#include "level_builder.h"
#include "renderer/color.h"
#include "core/platform.h"

#include <vector>
#include <thread>
#include <mutex>

static void CreateSurfaceTexCoords(Surface* surface, const Tileset* tileset, s32 tileId)
{
    glm::vec2 min, max;
    Tileset_GetTileUVs(tileset, tileId, &min, &max);
    surface->texcoords[0] = glm::vec2(min.x, min.y);
    surface->texcoords[1] = glm::vec2(max.x, min.y);
    surface->texcoords[2] = glm::vec2(max.x, max.y);
    surface->texcoords[3] = glm::vec2(min.x, max.y);

    /*
    surface->texcoords[0] = glm::vec2(0.0f, 0.0f);
    surface->texcoords[1] = glm::vec2(1.0f, 0.0f);
    surface->texcoords[2] = glm::vec2(1.0f, 1.0f);
    surface->texcoords[3] = glm::vec2(0.0f, 1.0f);

    if (surface->texture & 0x80000000)
    {
        surface->texture &= ~0x80000000;
        surface->texcoords[0] = glm::vec2(1.0f, 0.0f);
        surface->texcoords[1] = glm::vec2(1.0f, 1.0f);
        surface->texcoords[2] = glm::vec2(0.0f, 1.0f);
        surface->texcoords[3] = glm::vec2(0.0f, 0.0f);
    }
    */
}

static void CreateFloorVertices(Surface* surface, glm::vec2 position, const Tileset* tileset, s32 tileId = 0)
{
    surface->vertices[0] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 0.0f);
    surface->vertices[1] = glm::vec3(position.x + 0.0f, 0.0f, position.y + 1.0f);
    surface->vertices[2] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 1.0f);
    surface->vertices[3] = glm::vec3(position.x + 1.0f, 0.0f, position.y + 0.0f);

    glm::vec3 v1 = surface->vertices[1] - surface->vertices[0];
    glm::vec3 v2 = surface->vertices[2] - surface->vertices[0];
    surface->normal = glm::normalize(glm::cross(v1, v2));

    CreateSurfaceTexCoords(surface, tileset, tileId);
}

static void CreateCeilingVertices(Surface* surface, glm::vec2 position, const Tileset* tileset, s32 tileId = 0)
{
    surface->vertices[0] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 0.0f);
    surface->vertices[1] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 0.0f);
    surface->vertices[2] = glm::vec3(position.x + 1.0f, 1.0f, position.y + 1.0f);
    surface->vertices[3] = glm::vec3(position.x + 0.0f, 1.0f, position.y + 1.0f);

    glm::vec3 v1 = surface->vertices[1] - surface->vertices[0];
    glm::vec3 v2 = surface->vertices[2] - surface->vertices[0];
    surface->normal = glm::normalize(glm::cross(v1, v2));

    CreateSurfaceTexCoords(surface, tileset, tileId);
}

static void CreateWallVertices(Surface* surface, WallDirection direction, glm::vec2 position, const Tileset* tileset, s32 tileId = 0)
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

    CreateSurfaceTexCoords(surface, tileset, tileId);
}

Surface* CreateLevelSurfaces(const Level* level, const Tileset* tileset, Arena* transientStorage, s32* outSurfaceCount)
{
    *outSurfaceCount = 0;
    const u64 MAX_SURFACES = level->width * level->height * 4;
    Surface *surfaces = (Surface*)Arena_PushSize(transientStorage, sizeof(Surface) * MAX_SURFACES);
    s32 surfaceIndex = 0;

    for (s32 layer = 0; layer < Layer_Count; ++layer)
    {
        for (s32 y = 0; y < level->height; ++y)
        {
            for (s32 x = 0; x < level->width; ++x)
            {
                u32 data = Level_GetTileAt(level, x, y, (Layer)layer);
                s32 tileId = (s32)data - 1;
                switch (layer)
                {
                    case Layer_Floor: {
                        if (data == 0) continue;
                        Surface& surface = surfaces[surfaceIndex++];
                        surface.texture = data - 1;
                        CreateFloorVertices(&surface, glm::vec2(x, y), tileset, tileId);
                    } break;

                    case Layer_Ceiling: {
                        if (data == 0) continue;
                        Surface& surface = surfaces[surfaceIndex++];
                        surface.texture = data - 1;
                        CreateCeilingVertices(&surface, glm::vec2(x, y), tileset, tileId);
                    } break;

                    case Layer_Wall: {
                        if (data == 0) continue;
                        u32 northData = Level_GetTileAt(level, x, y + 1, (Layer)layer);
                        if (northData == 0)
                        {
                            Surface& surface = surfaces[surfaceIndex++];
                            surface.texture = data - 1;
                            CreateWallVertices(&surface, WallDirection_North, glm::vec2(x, y), tileset, tileId);
                        }

                        u32 southData = Level_GetTileAt(level, x, y - 1, (Layer)layer);
                        if (southData == 0)
                        {
                            Surface& surface = surfaces[surfaceIndex++];
                            surface.texture = data - 1;
                            CreateWallVertices(&surface, WallDirection_South, glm::vec2(x, y), tileset, tileId);
                        }
                        
                        u32 eastData = Level_GetTileAt(level, x + 1, y, (Layer)layer);
                        if (eastData == 0)
                        {
                            Surface& surface = surfaces[surfaceIndex++];
                            surface.texture = data - 1;
                            CreateWallVertices(&surface, WallDirection_East, glm::vec2(x, y), tileset, tileId);
                        }
                        u32 westData = Level_GetTileAt(level, x - 1, y, (Layer)layer);
                        if (westData == 0)
                        {
                            Surface& surface = surfaces[surfaceIndex++];
                            surface.texture = data - 1;
                            CreateWallVertices(&surface, WallDirection_West, glm::vec2(x, y), tileset, tileId);
                        }                        
                    } break;
                }
            }
        }
    }

    *outSurfaceCount = surfaceIndex;

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

f32 ComputeAttenuation(f32 distance, f32 range)
{
    f32 lConstant = 1.0f;
    f32 lLinear = 4.5f / range;
    f32 lQuadratic = 75.0f / (range * range);
    f32 attenuation = powf(1.0f / (lConstant + lLinear * distance + lQuadratic * (distance * distance)), 1.5f);
    return attenuation;
}

f32 ComputeLightFallOff(const Light* light, f32 distance)
{
    f32 lightFalloff = 1.0f - (distance / light->range);
    lightFalloff = glm::clamp(lightFalloff, 0.0f, 1.0f);
    return lightFalloff;
}

glm::vec4 ComputeLuxelLighting(const Level* level, glm::vec3 luxelPosition, const Light* light, const Atlas* atlas, s32 surfaceIndex, s32 x, s32 y)
{
    glm::vec3 ray = glm::normalize(luxelPosition - light->position);
    glm::vec2 direction = glm::normalize(glm::vec2(ray.x, ray.z));
    glm::vec2 origin = glm::vec2(light->position.x, light->position.z) + direction * 0.01f;

    f32 luxelDist = glm::distance(glm::vec2(luxelPosition.x, luxelPosition.z), glm::vec2(light->position.x, light->position.z));

    // Get the current color in the lightmap
    glm::ivec2 pixelPosition = Atlas_GetTileMinAt(atlas, surfaceIndex);
    pixelPosition.x += x;
    pixelPosition.y += y;
    u32 prevPixel = ((u32*)atlas->image.pixels)[pixelPosition.x + pixelPosition.y * atlas->image.width];
    glm::vec4 color = Color_ConvertToVec4(Color_ConvertToRGBA(prevPixel, PixelFormat_ABGR));

    RayCastHit hit = {};
    if (Level_CastRay(level, origin, direction, &hit))
    {
        if (hit.distance + 0.02f > luxelDist)
        {
            luxelDist = glm::distance(luxelPosition, light->position);
            //f32 attenuation = ComputeAttenuation(luxelDist, light->range);
            f32 attenuation = ComputeLightFallOff(light, luxelDist);

            color += glm::vec4(light->color * light->intensity * attenuation, 1.0f);
            color = glm::clamp(color, 0.0f, 1.0f);
        }
    }

    return color;
}

glm::vec4 ComputeLuxelLighting(const Level* level, const Light* light, const glm::vec3& luxelPosition)
{
    glm::vec3 ray = glm::normalize(luxelPosition - light->position);
    glm::vec2 direction = glm::normalize(glm::vec2(ray.x, ray.z));
    glm::vec2 origin = glm::vec2(light->position.x, light->position.z) + direction * 0.01f;

    f32 luxelDist = glm::distance(glm::vec2(luxelPosition.x, luxelPosition.z), glm::vec2(light->position.x, light->position.z));

    glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    RayCastHit hit = {};
    if (Level_CastRay(level, origin, direction, &hit))
    {
        if (hit.distance + 0.02f > luxelDist)
        {
            luxelDist = glm::distance(luxelPosition, light->position);
            f32 attenuation = ComputeLightFallOff(light, luxelDist);
            color = glm::vec4(light->color * light->intensity * attenuation, 1.0f);
            color = glm::clamp(color, 0.0f, 1.0f);
        }
    }

    return color;
}

void DilateTile(Image* image, glm::ivec2 tileMin, s32 tileWidth, s32 tileHeight, s32 padding)
{
    for (s32 i = 1; i <= padding; i++)
    {
        // Dilate the top and bottom row
        for (s32 x = 0; x < tileWidth; ++x)
        {
            Image_CopyPixel(image, tileMin.x + x, tileMin.y, tileMin.x + x, tileMin.y - i);
            Image_CopyPixel(image, tileMin.x + x, tileMin.y + tileHeight - 1, tileMin.x + x, tileMin.y + tileHeight - 1 + i);
        }

        // Dilate the left and right column
        for (s32 y = 0; y < tileHeight + padding * 2; ++y)
        {
            Image_CopyPixel(image, tileMin.x, tileMin.y + y, tileMin.x - i, tileMin.y + y);
            Image_CopyPixel(image, tileMin.x + tileWidth - 1, tileMin.y + y, tileMin.x + tileWidth - 1 + i, tileMin.y + y);
        }

        // Dilate the corners
        Image_CopyPixel(image, tileMin.x, tileMin.y, tileMin.x - i, tileMin.y - i); // top left
        Image_CopyPixel(image, tileMin.x + tileWidth - 1, tileMin.y, tileMin.x + tileWidth - 1 + i, tileMin.y - i); // top right
        Image_CopyPixel(image, tileMin.x, tileMin.y + tileHeight - 1, tileMin.x - i, tileMin.y + tileHeight - 1 + i); // bottom left
        Image_CopyPixel(image, tileMin.x + tileWidth - 1, tileMin.y + tileHeight - 1, tileMin.x + tileWidth - 1 + i, tileMin.y + tileHeight - 1 + i); // bottom right
    }
}

void CreateLightmapForSurface(Image* image, const Surface* surface, const Level* level, const Light* light, s32 luxelsPerRow, s32 luxelsPerCol, s32 padding)
{
    Image_FillColor(image, Color{ 0, 0, 0, 255 });

    glm::vec3 uAxis = glm::normalize(surface->vertices[1] - surface->vertices[0]);
    glm::vec3 vAxis = glm::normalize(surface->vertices[3] - surface->vertices[0]);

    for (s32 y = 0; y < luxelsPerCol + padding * 2; ++y)
    {
        for (s32 x = 0; x < luxelsPerRow + padding * 2; ++x)
        {
            f32 xOffset = ((f32)x + 0.5f - padding) / (luxelsPerRow + padding * 2 - 2 * padding);
            f32 yOffset = ((f32)y + 0.5f - padding) / (luxelsPerCol + padding * 2 - 2 * padding);

            glm::vec3 luxelPosition = surface->vertices[0] + uAxis * xOffset + vAxis * yOffset;
            glm::vec4 luxelColor = ComputeLuxelLighting(level, light, luxelPosition);

            Color color = Color_ConvertToColor(luxelColor);
            u32 pixelColor = Color_ConvertToU32(color, PixelFormat_ABGR);
            ((u32*)image->pixels)[(y * image->width) + x] = pixelColor;
        }
    }
}

static std::mutex atlasMutex;

void ComputeLightmap(Atlas* atlas, const Level* level, const Surface* surfaces, s32 surfaceCount, const Light* lights, s32 lightCount)
{
    f32 ambientLight = 0.1f;

    Image_FillColor(&atlas->image, Color_ConvertToColor(glm::vec4(glm::vec3(ambientLight), 1.0f)));

    s32 tileWidth = (atlas->tileWidth + atlas->padding * 2);
    s32 tileHeight = (atlas->tileHeight + atlas->padding * 2);

    for (s32 i = 0; i < lightCount; ++i)
    {
        const Light* light = &lights[i];

        std::vector<std::thread> threads;

        for (s32 j = 0; j < surfaceCount; ++j)
        {
            threads.emplace_back([=]() {
                const Surface* surface = &surfaces[j];
            
                Image tImage = {};
                tImage.width = tileWidth;
                tImage.height = tileHeight;
                tImage.pixels = Platform_Alloc(sizeof(u32) * tImage.width * tImage.height);

                Image tImage2 = {};
                tImage2.width = tileWidth * 2;
                tImage2.height = tileHeight * 2;
                tImage2.pixels = Platform_Alloc(sizeof(u32) * tImage2.width * tImage2.height);

                Image tImage3 = {};
                tImage3.width = tileWidth;
                tImage3.height = tileHeight;
                tImage3.pixels = Platform_Alloc(sizeof(u32) * tImage3.width * tImage3.height);

                s32 luxelsPerRow = atlas->tileWidth * 2;
                s32 luxelsPerCol = atlas->tileHeight * 2;
                s32 lightmapPadding = atlas->padding * 2;

                CreateLightmapForSurface(&tImage2, surface, level, light, luxelsPerRow, luxelsPerCol, lightmapPadding);
                Image_Downsample2x(&tImage, &tImage2);

                glm::ivec2 tileMin = Atlas_GetTileMinAt(atlas, j);
                {
                    std::lock_guard<std::mutex> lock(atlasMutex);

                    Image_Crop(&tImage3, &atlas->image, tileMin.x, tileMin.y, tileWidth, tileHeight);
                    Image_AddImages(&tImage, &tImage3, &tImage);
                    Image_Blit(&atlas->image, &tImage, tileMin.x, tileMin.y);
                }
                

                Platform_Free(tImage.pixels);
                Platform_Free(tImage2.pixels);
                Platform_Free(tImage3.pixels);
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }
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