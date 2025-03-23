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

f32 DegToRad(f32 degrees)
{
    f32 result = degrees * PI / 180.0f;
    return result;
}

f32 RadToDeg(f32 radians)
{
    f32 result = radians * 180.0f / PI;
    return result;
}

f32 Sin(f32 a)
{
    f32 result = sinf(a);
    return result;
}

f32 Cos(f32 a)
{
    f32 result = cosf(a);
    return result;
}

f32 Tan(f32 a)
{
    f32 result = tanf(a);
    return result;
}

f32 Atan2(f32 y, f32 x)
{
    f32 result = atan2f(y, x);
    return result;
}

f32 Abs(f32 a)
{
    f32 result = fabsf(a);
    return result;
}

f32 Min(f32 a, f32 b)
{
    f32 result = a < b ? a : b;
    return result;
}

f32 Max(f32 a, f32 b)
{
    f32 result = a > b ? a : b;
    return result;
}

f32 Clamp(f32 a, f32 min, f32 max)
{
    f32 result = Min(Max(a, min), max);
    return result;

}

f32 Lerp(f32 a, f32 b, f32 t)
{
    f32 result = a + (b - a) * t;
    return result;
}
