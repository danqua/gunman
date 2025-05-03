#pragma once
#include "core/types.h"
#include "renderer/color.h"

struct Image
{
    s32 width;
    s32 height;
    void* pixels;
};

// Loads an image from a file and returns an Image struct.
Image Image_LoadFromFile(const char* filename);

// Frees the memory used by an Image struct.
void Image_Free(Image* image);

// Saves an Image struct to a file.
b32 Image_SaveToFile(const char* filename, Image* image);

// Fills an image with a solid color.
void Image_FillColor(Image* image, Color color);

// Blits a source image onto a destination image at the specified position.
void Image_Blit(Image* dest, Image* src, s32 x, s32 y);