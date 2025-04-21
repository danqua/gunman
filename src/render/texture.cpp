#include "texture.h"
#include "image.h"
#include "platform/platform.h"
#include <glad/glad.h>

constexpr u32 kMaxTextures = 128;

struct Texture
{
    u32 id;
    s32 width;
    s32 height;
};

static Texture textures[kMaxTextures];

Texture* GetNextAvailableTexture()
{
    for (u32 i = 0; i < kMaxTextures; i++)
    {
        if (textures[i].id == 0)
        {
            return &textures[i];
        }
    }
    return nullptr;
}

Texture* TextureCreate(const void* data, s32 width, s32 height)
{
    Texture* texture = GetNextAvailableTexture();

    if (!texture)
    {
        PlatformLog("Failed to create texture: No available slots\n");
        return nullptr;
    }

    texture->width = width;
    texture->height = height;

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    return texture;
}

Texture* TextureLoad(const char* path)
{
    Image image = ImageLoad(path);
    if (image.pixels == nullptr)
    {
        PlatformLog("Failed to load texture: %s\n", path);
        return nullptr;
    }

    Texture* texture = TextureCreate(image.pixels, image.width, image.height);
    if (texture == nullptr)
    {
        PlatformLog("Failed to create texture: %s\n", path);
        return nullptr;
    }

    ImageFree(&image);
    return texture;
}

void TextureDestroy(Texture* texture)
{
    glDeleteTextures(1, &texture->id);
    texture->id = 0;
    texture->width = 0;
    texture->height = 0;
}

void TextureBind(Texture* texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void TextureUnbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

s32 TextureGetWidth(Texture* texture)
{
    return texture->width;
}

s32 TextureGetHeight(Texture* texture)
{
    return texture->height;
}