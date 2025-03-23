#include "math.h"

Vec2 Vec2Zero()
{
    Vec2 result;
    result.x = 0.0f;
    result.y = 0.0f;
    return result;
}

Vec2 Vec2Add(Vec2 a, Vec2 b)
{
    Vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

Vec2 Vec2Subtract(Vec2 a, Vec2 b)
{
    Vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

Vec2 Vec2Multiply(Vec2 a, f32 b)
{
    Vec2 result;
    result.x = a.x * b;
    result.y = a.y * b;
    return result;
}

Vec2 Vec2Divide(Vec2 a, f32 b)
{
    Vec2 result;
    result.x = a.x / b;
    result.y = a.y / b;
    return result;
}

f32 Vec2Dot(Vec2 a, Vec2 b)
{
    f32 result = a.x * b.x + a.y * b.y;
    return result;
}

f32 Vec2Length(Vec2 a)
{
    f32 result = sqrtf(a.x * a.x + a.y * a.y);
    return result;
}

f32 Vec2LengthSquared(Vec2 a)
{
    f32 result = a.x * a.x + a.y * a.y;
    return result;
}

Vec2 Vec2Normalize(Vec2 a)
{
    f32 length = Vec2Length(a);
    Vec2 result = length > 1e-4f ? Vec2Divide(a, length) : Vec2Zero();
    return result;
}