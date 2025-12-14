#pragma once
#include "core/types.h"
#include "renderer/rhi.h"

struct Sprite
{
    s32 xOffset;
    s32 yOffset;
    s32 width;
    s32 height;
    TextureId texture;
};

struct SpriteAnimation
{
    f32 frameTime;
    u32 frameCount;
    Sprite* sprites[8];
    bool loop;
};