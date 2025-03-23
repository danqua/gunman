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


// ----------------------------------------------------------------
// Math functions
// ----------------------------------------------------------------

// Pi constant
#define PI 3.14159265359f

// Converts degrees to radians
f32 DegToRad(f32 degrees);

// Converts radians to degrees
f32 RadToDeg(f32 radians);

// Sinus function
f32 Sin(f32 a);

// Cosinus function
f32 Cos(f32 a);

// Tangent function
f32 Tan(f32 a);

// Arc tangent function
f32 Atan2(f32 y, f32 x);

// Returns the absolute value of a number
f32 Abs(f32 a);

// Returns the minimum of two numbers
f32 Min(f32 a, f32 b);

// Returns the maximum of two numbers
f32 Max(f32 a, f32 b);

// Clamps a number between a min and max value
f32 Clamp(f32 a, f32 min, f32 max);

// Linearly interpolates between two numbers
f32 Lerp(f32 a, f32 b, f32 t);
