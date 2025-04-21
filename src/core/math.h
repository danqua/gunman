#pragma once
#include <math.h>

#include "core/types.h"

// ----------------------------------------------------------------
// Vec2
// ----------------------------------------------------------------
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

// Returns the distance between two Vec2s
f32 Vec2Distance(Vec2 a, Vec2 b);

// Returns a normalized Vec2
Vec2 Vec2Normalize(Vec2 a);

// Returns the minimum of two Vec2s
Vec2 Vec2Min(Vec2 a, Vec2 b);

// Returns the maximum of two Vec2s
Vec2 Vec2Max(Vec2 a, Vec2 b);

// Clamps a Vec2 between a min and max value
Vec2 Vec2Clamp(Vec2 a, Vec2 min, Vec2 max);

// Returns the angle between two Vec2s in radians
f32 Vec2Angle(Vec2 a, Vec2 b);

// Returns true if two Vec2s are equal
b32 Vec2Equal(Vec2 a, Vec2 b);

// Returns a rotated Vec2
Vec2 Vec2Rotate(Vec2 a, f32 angle);

// Linear interpolation between two Vec2s
Vec2 Vec2Lerp(Vec2 a, Vec2 b, f32 t);

// ----------------------------------------------------------------
// Vec3
// ----------------------------------------------------------------
struct Vec3
{
    f32 x;
    f32 y;
    f32 z;
};

// Returns a Vec3 with x, y and z set to 0.0f
Vec3 Vec3Zero();

// Adds two Vec3s together
Vec3 Vec3Add(Vec3 a, Vec3 b);

// Subtracts two Vec3s
Vec3 Vec3Subtract(Vec3 a, Vec3 b);

// Multiplies a Vec3 by a scalar
Vec3 Vec3Multiply(Vec3 a, f32 b);

// Divides a Vec3 by a scalar
Vec3 Vec3Divide(Vec3 a, f32 b);

// Returns the dot product of two Vec3s
f32 Vec3Dot(Vec3 a, Vec3 b);

// Returns the cross product of two Vec3s
Vec3 Vec3Cross(Vec3 a, Vec3 b);

// Returns the length of a Vec3
f32 Vec3Length(Vec3 a);

// Returns the squared length of a Vec3
f32 Vec3LengthSquared(Vec3 a);

// Returns a normalized Vec3
Vec3 Vec3Normalize(Vec3 a);

// Returns the minimum of two Vec3s
Vec3 Vec3Min(Vec3 a, Vec3 b);

// Returns the maximum of two Vec3s
Vec3 Vec3Max(Vec3 a, Vec3 b);

// Clamps a Vec3 between a min and max value
Vec3 Vec3Clamp(Vec3 a, Vec3 min, Vec3 max);

// ----------------------------------------------------------------
// Vec4
// ----------------------------------------------------------------
struct Vec4
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

// Returns a Vec4 with x, y, z and w set to 0.0f
Vec4 Vec4Zero();

// Adds two Vec4s together
Vec4 Vec4Add(Vec4 a, Vec4 b);

// Subtracts two Vec4s
Vec4 Vec4Subtract(Vec4 a, Vec4 b);

// Multiplies a Vec4 by a scalar
Vec4 Vec4Multiply(Vec4 a, f32 b);

// Divides a Vec4 by a scalar
Vec4 Vec4Divide(Vec4 a, f32 b);

// Returns the dot product of two Vec4s
f32 Vec4Dot(Vec4 a, Vec4 b);

// Returns the length of a Vec4
f32 Vec4Length(Vec4 a);

// Returns the squared length of a Vec4
f32 Vec4LengthSquared(Vec4 a);

// Returns a normalized Vec4
Vec4 Vec4Normalize(Vec4 a);

// Returns the minimum of two Vec4s
Vec4 Vec4Min(Vec4 a, Vec4 b);

// Returns the maximum of two Vec4s
Vec4 Vec4Max(Vec4 a, Vec4 b);

// Clamps a Vec4 between a min and max value
Vec4 Vec4Clamp(Vec4 a, Vec4 min, Vec4 max);

// ----------------------------------------------------------------
// Mat4
// ----------------------------------------------------------------
struct Mat4
{
    f32 m[4][4];
};

// Returns an identity matrix
Mat4 Mat4Identity();

// Multiplies two matrices
Mat4 Mat4Multiply(Mat4 a, Mat4 b);

// Returns a translation matrix
Mat4 Mat4Translate(Mat4 mat, Vec3 position);

// Returns a scaling matrix
Mat4 Mat4Scale(Mat4 mat, Vec3 scale);

// Returns a rotation matrix
Mat4 Mat4Rotate(Mat4 mat, f32 angle, Vec3 axis);

// Look at matrix
Mat4 Mat4LookAt(Vec3 eye, Vec3 target, Vec3 up);

// Perspective projection matrix
Mat4 Mat4Perspective(f32 fov, f32 aspect, f32 near, f32 far);

// Orthographic projection matrix
Mat4 Mat4Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

// ----------------------------------------------------------------
// Box2
// ----------------------------------------------------------------
struct Box2
{
    Vec2 min;
    Vec2 max;
};

// Returns true if a box contains a point
b32 Box2ContainsPoint(Box2 box, Vec2 point);

// Returns true if two boxes overlap
b32 Box2Overlap(Box2 a, Box2 b);

// Returns the center of a box
Vec2 Box2Center(Box2 box);

// ----------------------------------------------------------------
// Box3
// ----------------------------------------------------------------
struct Box3
{
    Vec3 min;
    Vec3 max;
};

// ----------------------------------------------------------------
// Point2D
// ----------------------------------------------------------------
struct Point2D
{
    s32 x;
    s32 y;
};

// ----------------------------------------------------------------
// Math utility functions
// ----------------------------------------------------------------

// Pi constant
#define PI 3.14159265359f

// Converts degrees to radians
f32 DegToRad(f32 degrees);

// Converts radians to degrees
f32 RadToDeg(f32 radians);

// Returns the floor of a number
f32 Floor(f32 a);

// Returns the ceiling of a number
f32 Ceil(f32 a);

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

// Squared root function
f32 SquareRoot(f32 a);
