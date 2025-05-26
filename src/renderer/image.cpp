#include "image.h"
#include "core/platform.h"
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Image Image_LoadFromFile(const char* filename)
{
    stbi_set_flip_vertically_on_load(true);

    Image image = {};
    image.pixels = stbi_load(filename, &image.width, &image.height, NULL, STBI_rgb_alpha);

    if (!image.pixels)
    {
        printf("Failed to load image: %s\n", filename);
        return Image{};
    }

    return image;
}

void Image_Free(Image* image)
{
    if (image->pixels)
    {
        stbi_image_free(image->pixels);
        image->pixels = nullptr;
        image->width = 0;
        image->height = 0;
    }
}

b32 Image_SaveToFile(const char* filename, Image* image)
{
    s32 result = stbi_write_png(filename, image->width, image->height, STBI_rgb_alpha, image->pixels, image->width * STBI_rgb_alpha);
    if (result == 0)
    {
        printf("Failed to save image: %s\n", filename);
        return false;
    }
    return true;
}

void Image_FillColor(Image* image, Color color)
{
    u32* pixels = (u32*)image->pixels;
    for (s32 i = 0; i < image->width * image->height; ++i)
    {
        pixels[i] = (color.a << 24) | (color.b << 16) | (color.g << 8) | color.r;
    }
}

void Image_Blit(Image* dest, Image* src, s32 x, s32 y)
{
    u32* destPixels = (u32*)dest->pixels;
    u32* srcPixels = (u32*)src->pixels;

    for (s32 sy = 0; sy < src->height; ++sy)
    {
        for (s32 sx = 0; sx < src->width; ++sx)
        {
            s32 destX = x + sx;
            s32 destY = y + sy;
            if (destX < 0 || destX >= dest->width || destY < 0 || destY >= dest->height)
            {
                continue;
            }
            destPixels[destX + destY * dest->width] = srcPixels[sx + sy * src->width];
        }
    }
}

void Image_CopyPixel(Image* image, s32 srcX, s32 srcY, s32 dstX, s32 dstY)
{
    if (srcX < 0 || srcX >= image->width || srcY < 0 || srcY >= image->height ||
        dstX < 0 || dstX >= image->width || dstY < 0 || dstY >= image->height)
    {
        return;
    }
    u32* pixels = (u32*)image->pixels;
    u32 color = pixels[srcX + srcY * image->width];
    pixels[dstX + dstY * image->width] = color;
}

void Image_Downsample2x(Image* dst, const Image* src)
{
    for (s32 y = 0; y < dst->height; ++y)
    {
        for (s32 x = 0; x < dst->width; ++x)
        {
            u32 r = 0, g = 0, b = 0, a = 0;

            for (s32 dy = 0; dy < 2; ++dy)
            {
                for (s32 dx = 0; dx < 2; ++dx)
                {
                    s32 sx = x * 2 + dx;
                    s32 sy = y * 2 + dy;

                    u32 pixel = ((u32*)src->pixels)[sy * src->width + sx];
                    Color c = Color_ConvertToRGBA(pixel, PixelFormat_ABGR);

                    r += c.r;
                    g += c.g;
                    b += c.b;
                    a += c.a; // oder: max(a, c.a) falls du Pre-Multiplied Alpha machst
                }
            }

            Color avg = {
                (u8)(r / 4),
                (u8)(g / 4),
                (u8)(b / 4),
                (u8)(a / 4)
            };

            ((u32*)dst->pixels)[y * dst->width + x] = Color_ConvertToU32(avg, PixelFormat_ABGR);
        }
    }
}

void Image_AddImages(Image* dst, const Image* a, const Image* b)
{
    for (s32 y = 0; y < dst->height; ++y)
    {
        for (s32 x = 0; x < dst->width; ++x)
        {
            u32 pixelA = ((u32*)a->pixels)[y * a->width + x];
            u32 pixelB = ((u32*)b->pixels)[y * b->width + x];

            Color colorA = Color_ConvertToRGBA(pixelA, PixelFormat_ABGR);
            Color colorB = Color_ConvertToRGBA(pixelB, PixelFormat_ABGR);

            Color resultColor = {
                (u8)glm::clamp((colorA.r + colorB.r), 0, 255),
                (u8)glm::clamp((colorA.g + colorB.g), 0, 255),
                (u8)glm::clamp((colorA.b + colorB.b), 0, 255),
                (u8)glm::clamp((colorA.a + colorB.a), 0, 255)
            };

            ((u32*)dst->pixels)[y * dst->width + x] = Color_ConvertToU32(resultColor, PixelFormat_ABGR);
        }
    }
}

void Image_Crop(Image* dst, const Image* src, s32 x, s32 y, s32 width, s32 height)
{
    s32 minX = x;
    s32 minY = y;
    s32 maxX = x + width;
    s32 maxY = y + height;

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX > src->width) maxX = src->width;
    if (maxY > src->height) maxY = src->height;
    if (minX >= maxX || minY >= maxY)
    {
        return; // No valid area to crop
    }

    for (s32 dstY = 0; dstY < height; ++dstY)
    {
        for (s32 dstX = 0; dstX < width; ++dstX)
        {
            s32 srcX = minX + dstX;
            s32 srcY = minY + dstY;

            if (srcX < 0 || srcX >= src->width || srcY < 0 || srcY >= src->height)
            {
                continue;
            }

            u32 pixel = ((u32*)src->pixels)[srcY * src->width + srcX];
            ((u32*)dst->pixels)[dstY * dst->width + dstX] = pixel;
        }
    }
}
