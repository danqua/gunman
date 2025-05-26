#pragma once
#include "renderer/rhi.h"

ShaderId LoadShaderFromFile(const char* vsFilename, const char* fsFilename);

TextureId LoadTextureFromFile(const char* filename);