#include "math.h"

// ----------------------------------------------------------------
// Vec2
// ----------------------------------------------------------------

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
    f32 result = SquareRoot(a.x * a.x + a.y * a.y);
    return result;
}

f32 Vec2LengthSquared(Vec2 a)
{
    f32 result = a.x * a.x + a.y * a.y;
    return result;
}

f32 Vec2Distance(Vec2 a, Vec2 b)
{
    f32 result = Vec2Length(Vec2Subtract(a, b));
    return result;
}

Vec2 Vec2Normalize(Vec2 a)
{
    f32 length = Vec2Length(a);
    Vec2 result = length > 1e-4f ? Vec2Divide(a, length) : Vec2Zero();
    return result;
}

Vec2 Vec2Min(Vec2 a, Vec2 b)
{
    Vec2 result;
    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    return result;
}

Vec2 Vec2Max(Vec2 a, Vec2 b)
{
    Vec2 result;
    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    return result;
}

Vec2 Vec2Clamp(Vec2 a, Vec2 min, Vec2 max)
{
    Vec2 result;
    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    return result;
}

// ----------------------------------------------------------------
// Vec3
// ----------------------------------------------------------------

Vec3 Vec3Zero()
{
    Vec3 result;
    result.x = 0.0f;
    result.y = 0.0f;
    result.z = 0.0f;
    return result;
}

Vec3 Vec3Add(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vec3 Vec3Subtract(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

Vec3 Vec3Multiply(Vec3 a, f32 b)
{
    Vec3 result;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    return result;
}

Vec3 Vec3Divide(Vec3 a, f32 b)
{
    Vec3 result;
    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    return result;
}

f32 Vec3Dot(Vec3 a, Vec3 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

Vec3 Vec3Cross(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

f32 Vec3Length(Vec3 a)
{
    f32 result = SquareRoot(a.x * a.x + a.y * a.y + a.z * a.z);
    return result;
}

f32 Vec3LengthSquared(Vec3 a)
{
    f32 result = a.x * a.x + a.y * a.y + a.z * a.z;
    return result;
}

Vec3 Vec3Normalize(Vec3 a)
{
    f32 length = Vec3Length(a);
    Vec3 result = length > 1e-4f ? Vec3Divide(a, length) : Vec3Zero();
    return result;
}

Vec3 Vec3Min(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);
    return result;
}

Vec3 Vec3Max(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);
    return result;
}

Vec3 Vec3Clamp(Vec3 a, Vec3 min, Vec3 max)
{
    Vec3 result;
    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    result.z = Clamp(a.z, min.z, min.z);
    return result;
}

// ----------------------------------------------------------------
// Vec4
// ----------------------------------------------------------------

Vec4 Vec4Zero()
{
    Vec4 result;
    result.x = 0.0f;
    result.y = 0.0f;
    result.z = 0.0f;
    result.w = 0.0f;
    return result;
}

Vec4 Vec4Add(Vec4 a, Vec4 b)
{
    Vec4 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

Vec4 Vec4Subtract(Vec4 a, Vec4 b)
{
    Vec4 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    return result;
}

Vec4 Vec4Multiply(Vec4 a, f32 b)
{
    Vec4 result;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    result.w = a.w * b;
    return result;
}

Vec4 Vec4Divide(Vec4 a, f32 b)
{
    Vec4 result;
    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    result.w = a.w / b;
    return result;
}

f32 Vec4Dot(Vec4 a, Vec4 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

f32 Vec4Length(Vec4 a)
{
    f32 result = SquareRoot(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
    return result;
}

f32 Vec4LengthSquared(Vec4 a)
{
    f32 result = a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
    return result;
}

Vec4 Vec4Normalize(Vec4 a)
{
    Vec4 result;
    f32 length = Vec4Length(a);
    result = length > 1e-4f ? Vec4Divide(a, length) : Vec4Zero();
    return result;
}

Vec4 Vec4Min(Vec4 a, Vec4 b)
{
    Vec4 result;
    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);
    result.w = Min(a.w, b.w);
    return result;
}

Vec4 Vec4Max(Vec4 a, Vec4 b)
{
    Vec4 result;
    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);
    result.w = Max(a.w, b.w);
    return result;
}

Vec4 Vec4Clamp(Vec4 a, Vec4 min, Vec4 max)
{
    Vec4 result;
    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    result.z = Clamp(a.z, min.z, max.z);
    result.w = Clamp(a.w, min.w, max.w);
    return result;
}

// ----------------------------------------------------------------
// Mat4
// ----------------------------------------------------------------

Mat4 Mat4Identity()
{
    Mat4 result = {};
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    return result;
}

Mat4 Mat4Multiply(Mat4 a, Mat4 b)
{
    Mat4 result = {};

    for (s32 row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result.m[row][col] = 0.0f;
            for (s32 k = 0; k < 4; ++k)
            {
                result.m[row][col] += a.m[row][k] * b.m[k][col];
            }
        }
    }

    return result;
}

Mat4 Mat4Translate(Mat4 mat, Vec3 position)
{
    Mat4 result = mat;
    result.m[3][0] += position.x;
    result.m[3][1] += position.y;
    result.m[3][2] += position.z;
    return result;
}

Mat4 Mat4Scale(Mat4 mat, Vec3 scale)
{
    Mat4 result = mat;
    result.m[0][0] *= scale.x;
    result.m[1][1] *= scale.y;
    result.m[2][2] *= scale.z;
    return result;
}

Mat4 Mat4Rotate(Mat4 mat, f32 angle, Vec3 axis)
{
    f32 rad = DegToRad(angle);
    f32 c = Cos(rad);
    f32 s = Sin(rad);
    f32 one_minus_c = 1.0f - c;

    // Normalize the axis
    Vec3 normalized_axis = Vec3Normalize(axis);

    f32 x = normalized_axis.x;
    f32 y = normalized_axis.y;
    f32 z = normalized_axis.z;

    Mat4 rotation_matrix = Mat4Identity();
    rotation_matrix.m[0][0] = c + x * x * one_minus_c;
    rotation_matrix.m[0][1] = x * y * one_minus_c - z * s;
    rotation_matrix.m[0][2] = x * z * one_minus_c + y * s;

    rotation_matrix.m[1][0] = y * x * one_minus_c + z * s;
    rotation_matrix.m[1][1] = c + y * y * one_minus_c;
    rotation_matrix.m[1][2] = y * z * one_minus_c - x * s;

    rotation_matrix.m[2][0] = z * x * one_minus_c - y * s;
    rotation_matrix.m[2][1] = z * y * one_minus_c + x * s;
    rotation_matrix.m[2][2] = c + z * z * one_minus_c;

    // Multiply the current matrix by the rotation matrix
    return Mat4Multiply(mat, rotation_matrix);
}

Mat4 Mat4LookAt(Vec3 eye, Vec3 target, Vec3 up)
{
    Vec3 zaxis = Vec3Normalize(Vec3Subtract(eye, target)); // Forward
    Vec3 xaxis = Vec3Normalize(Vec3Cross(up, zaxis));      // Right
    Vec3 yaxis = Vec3Cross(zaxis, xaxis);                  // Up

    Mat4 result = Mat4Identity();
    result.m[0][0] = xaxis.x;
    result.m[1][0] = xaxis.y;
    result.m[2][0] = xaxis.z;
    result.m[0][1] = yaxis.x;
    result.m[1][1] = yaxis.y;
    result.m[2][1] = yaxis.z;
    result.m[0][2] = zaxis.x;
    result.m[1][2] = zaxis.y;
    result.m[2][2] = zaxis.z;

    result.m[3][0] = -Vec3Dot(xaxis, eye);
    result.m[3][1] = -Vec3Dot(yaxis, eye);
    result.m[3][2] = -Vec3Dot(zaxis, eye);
    
    return result;
}

Mat4 Mat4Perspective(f32 fov, f32 aspect, f32 near, f32 far)
{
    Mat4 result = {};

    f32 tan_half_fov = Tan(fov * 0.5f); // fov is in radians

    result.m[0][0] = 1.0f / (aspect * tan_half_fov);
    result.m[1][1] = 1.0f / tan_half_fov;
    result.m[2][2] = -(far + near) / (far - near);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

Mat4 Mat4Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    Mat4 result = {};

    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (far - near);
    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -(far + near) / (far - near);
    result.m[3][3] = 1.0f;

    return result;
}

// ----------------------------------------------------------------
// Box2
// ----------------------------------------------------------------

b32 Box2ContainsPoint(Box2 box, Vec2 point)
{
    b32 result = point.x >= box.min.x && point.x <= box.max.x &&
                 point.y >= box.min.y && point.y <= box.max.y;
    return result;
}

b32 Box2Overlap(Box2 a, Box2 b)
{
    b32 result = a.min.x <= b.max.x && a.max.x >= b.min.x &&
                 a.min.y <= b.max.y && a.max.y >= b.min.y;
    return result;
}

// ----------------------------------------------------------------
// Utility functions
// ----------------------------------------------------------------

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

f32 Floor(f32 a)
{
    return floorf(a);
}

f32 Ceil(f32 a)
{
    return ceilf(a);
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

f32 SquareRoot(f32 a)
{
    f32 result = sqrtf(a);
    return result;
}