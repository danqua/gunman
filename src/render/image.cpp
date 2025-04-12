#include "image.h"
#include "platform/platform.h"
#include <SDL3/SDL.h>


Image ImageLoad(const char* filename)
{
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface)
    {
        Image image = {};
        return image;
    }

    Image image = {};
    image.width = surface->w;
    image.height = surface->h;
    image.channels = 4;
    image.pixels = (u8*)PlatformAllocateMemory(image.width * image.height * image.channels);

    PlatformCopyMemory(image.pixels, surface->pixels, image.width * image.height * image.channels);

    SDL_DestroySurface(surface);

    return image;
}

void ImageFree(Image* image)
{
    PlatformFreeMemory(image->pixels);
    image->pixels = nullptr;
    image->width = 0;
    image->height = 0;
    image->channels = 0;
}