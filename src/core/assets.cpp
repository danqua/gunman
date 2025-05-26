#include "assets.h"
#include "core/platform.h"
#include "renderer/image.h"

ShaderId Asset_LoadShader(const char* vsFilename, const char* fsFilename)
{
    char* vsBuffer = nullptr;
    char* fsBuffer = nullptr;

    {
        FileHandle fileHandle = Platform_OpenFile(vsFilename, FileMode_Read);
        vsBuffer = (char*)Platform_Alloc(fileHandle.size + 1);
        vsBuffer[fileHandle.size] = 0;
        Platform_ReadDataFromFile(&fileHandle, vsBuffer, fileHandle.size);
        Platform_CloseFile(&fileHandle);
    }

    {
        FileHandle fileHandle = Platform_OpenFile(fsFilename, FileMode_Read);
        fsBuffer = (char*)Platform_Alloc(fileHandle.size + 1);
        fsBuffer[fileHandle.size] = 0;
        Platform_ReadDataFromFile(&fileHandle, fsBuffer, fileHandle.size);
        Platform_CloseFile(&fileHandle);
    }

    ShaderId shader = RHI_CreateShader(vsBuffer, fsBuffer);
    Platform_Free(vsBuffer);
    Platform_Free(fsBuffer);

    return shader;
}

void Asset_FreeShader(ShaderId shader)
{
    if (shader != 0)
    {
        RHI_DestroyShader(shader);
    }
}

TextureId Asset_LoadTexture(const char* filename)
{
    Image image = Image_LoadFromFile(filename);
    TextureId texture = RHI_CreateTexture(image.pixels, image.width, image.height, TextureFilter_Nearest);
    Image_Free(&image);
    return texture;
}

void Asset_FreeTexture(TextureId texture)
{
    if (texture != 0)
    {
        RHI_DestroyTexture(texture);
    }
}

AudioId Asset_LoadAudio(const char* filename)
{
    AudioId audio = Audio_LoadFromFile(filename);
    return audio;
}

void Asset_FreeAudio(AudioId audio)
{
    if (audio != 0)
    {
        Audio_Free(audio);
    }
}
