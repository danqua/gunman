#pragma once
#include "core/audio.h"
#include "core/memory.h"
#include "renderer/rhi.h"
#include "renderer/image.h"

// Loads a shader from file.
ShaderId Asset_LoadShader(const char* vsFilename, const char* fsFilename);

// Frees a shader from the asset manager.
void Asset_FreeShader(ShaderId shader);

// Loads a texture from file.
TextureId Asset_LoadTexture(const char* filename);

// Frees a texture from the asset manager.
void Asset_FreeTexture(TextureId texture);

// Loads an audio clip from file.
AudioId Asset_LoadAudio(const char* filename);

// Frees an audio clip from the asset manager.
void Asset_FreeAudio(AudioId audio);