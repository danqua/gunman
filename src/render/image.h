#pragma once
#include "core/types.h"

struct Image
{
    s32 width;
    s32 height;
    s32 channels;
    u8* pixels;
};

// Loads an image from a file.
Image ImageLoad(const char* filename);

// Frees the memory used by an image.
void ImageFree(Image* image);