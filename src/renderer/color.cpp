#include "color.h"

Color Color_ConvertToRGBA(u32 color, PixelFormat format)
{
    Color result = {};

    switch (format)
    {
        case PixelFormat_RGBA: {
            result.r = (color >> 24) & 0xFF;
            result.g = (color >> 16) & 0xFF;
            result.b = (color >> 8) & 0xFF;
            result.a = color & 0xFF;
        } break;

        case PixelFormat_ABGR: {
            result.a = (color >> 24) & 0xFF;
            result.b = (color >> 16) & 0xFF;
            result.g = (color >> 8) & 0xFF;
            result.r = color & 0xFF;
        } break;

        default: {
        
        } break;
    }

    return result;
}

glm::vec4 Color_ConvertToVec4(const Color& color)
{
    return glm::vec4(
        (f32)color.r / 255.0f,
        (f32)color.g / 255.0f,
        (f32)color.b / 255.0f,
        (f32)color.a / 255.0f
    );
}

u32 Color_ConvertToU32(const Color& color, PixelFormat format)
{
    u32 result = 0;

    switch (format)
    {
        case PixelFormat_RGBA: {
            result |= ((u32)color.r << 24);
            result |= ((u32)color.g << 16);
            result |= ((u32)color.b << 8);
            result |= (u32)color.a;
        } break;

        case PixelFormat_ABGR: {
            result |= ((u32)color.a << 24);
            result |= ((u32)color.b << 16);
            result |= ((u32)color.g << 8);
            result |= (u32)color.r;
        } break;

        default: {

        } break;
    }

    return result;
}

Color Color_ConvertToColor(const glm::vec4& color)
{
    Color result = {};

    result.r = (u8)(glm::clamp(color.r * 255.0f, 0.0f, 255.0f));
    result.g = (u8)(glm::clamp(color.g * 255.0f, 0.0f, 255.0f));
    result.b = (u8)(glm::clamp(color.b * 255.0f, 0.0f, 255.0f));
    result.a = (u8)(glm::clamp(color.a * 255.0f, 0.0f, 255.0f));

    return result;
}
