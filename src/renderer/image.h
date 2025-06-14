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

// Copies a pixel from one position to another in the same image.
void Image_CopyPixel(Image* image, s32 srcX, s32 srcY, s32 dstX, s32 dstY);

// Downsamples an image by a factor of 2.
void Image_Downsample2x(Image* dst, const Image* src);

// Adds two images together and stores the result in a destination image.
void Image_AddImages(Image* dst, const Image* a, const Image* b);

// Crops a region from the source image and stores it in the destination image.
void Image_Crop(Image* dst, const Image* src, s32 x, s32 y, s32 width, s32 height);