#pragma once
#include "core/types.h"

struct Texture;

// Creates a new texture.
Texture* TextureCreate(const void* data, s32 width, s32 height);

// Loads a texture from a file.
Texture* TextureLoad(const char* path);

// Destroys the texture.
void TextureDestroy(Texture* texture);

// Binds the texture.
void TextureBind(Texture* texture);

// Unbinds the texture.
void TextureUnbind();

// Returns the width of the texture.
s32 TextureGetWidth(Texture* texture);

// Returns the height of the texture.
s32 TextureGetHeight(Texture* texture);