#pragma once
#include "core/types.h"
#include <glm/glm.hpp>

struct Color
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

#define COLOR_WHITE           Color{ 255, 250, 235, 255 }   // Soft cream white
#define COLOR_BLACK           Color{  20,  20,  20, 255 }   // Dark black
#define COLOR_RED             Color{ 230, 100, 100, 255 }   // Muted red
#define COLOR_GREEN           Color{ 120, 200, 120, 255 }   // Soft green
#define COLOR_BLUE            Color{ 100, 150, 230, 255 }   // Muted blue
#define COLOR_YELLOW          Color{ 230, 220, 100, 255 }   // Warm yellow
#define COLOR_ORANGE          Color{ 240, 160, 100, 255 }   // Warm orange
#define COLOR_PURPLE          Color{ 170, 120, 230, 255 }   // Muted purple
#define COLOR_PINK            Color{ 230, 150, 180, 255 }   // Pastel pink
#define COLOR_CYAN            Color{ 120, 220, 220, 255 }   // Cool cyan
#define COLOR_MAGENTA         Color{ 200, 120, 200, 255 }   // Muted magenta
#define COLOR_BROWN           Color{ 160, 120,  90, 255 }   // Warm brown
#define COLOR_GRAY            Color{ 180, 180, 180, 255 }   // Neutral gray
#define COLOR_DARK_GRAY       Color{  90,  90,  90, 255 }   // Dark gray
#define COLOR_LIGHT_GRAY      Color{ 220, 220, 220, 255 }   // Light gray
#define COLOR_PASTEL_GREEN    Color{ 150, 230, 150, 255 }   // Pastel green
#define COLOR_PASTEL_BLUE     Color{ 150, 200, 230, 255 }   // Pastel blue
#define COLOR_PASTEL_YELLOW   Color{ 250, 250, 160, 255 }   // Pastel yellow
#define COLOR_PASTEL_ORANGE   Color{ 250, 200, 150, 255 }   // Pastel orange
#define COLOR_TEAL            Color{  90, 180, 180, 255 }   // Soft teal
#define COLOR_INDIGO          Color{ 120, 100, 210, 255 }   // Indigo
#define COLOR_MAROON          Color{ 150,  50,  90, 255 }   // Deep maroon
#define COLOR_OLIVE           Color{ 140, 140,  90, 255 }   // Olive

enum PixelFormat
{
    PixelFormat_RGBA,
    PixelFormat_ABGR,
};

Color Color_ConvertToRGBA(u32 color, PixelFormat format);

glm::vec4 Color_ConvertToVec4(const Color& color);
u32 Color_ConvertToU32(const Color& color, PixelFormat format = PixelFormat_RGBA);
Color Color_ConvertToColor(const glm::vec4& color);