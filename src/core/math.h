#pragma once
#include <math.h>

#include "core/types.h"

struct Vec2
{
    f32 x;
    f32 y;
};

// Returns a Vec2 with x and y set to 0.0f
Vec2 Vec2Zero();

// Adds two Vec2s together
Vec2 Vec2Add(Vec2 a, Vec2 b);

// Subtracts two Vec2s
Vec2 Vec2Subtract(Vec2 a, Vec2 b);

// Multiplies a Vec2 by a scalar
Vec2 Vec2Multiply(Vec2 a, f32 b);

// Divides a Vec2 by a scalar
Vec2 Vec2Divide(Vec2 a, f32 b);

// Returns the dot product of two Vec2s
f32 Vec2Dot(Vec2 a, Vec2 b);

// Returns the length of a Vec2
f32 Vec2Length(Vec2 a);

// Returns the squared length of a Vec2
f32 Vec2LengthSquared(Vec2 a);

// Returns a normalized Vec2
Vec2 Vec2Normalize(Vec2 a);