#include "image.h"

#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Image Image_LoadFromFile(const char* filename)
{
    //stbi_set_flip_vertically_on_load(true);

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
