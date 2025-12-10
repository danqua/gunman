#pragma once
#include "core/types.h"
#include <glm/gtc/matrix_transform.hpp>

struct Box2
{
    glm::vec2 min;
    glm::vec2 max;
};

struct Box3
{
    glm::vec3 min;
    glm::vec3 max;
};

struct Plane
{
    glm::vec3 normal;
    f32 d;
};

// Linear interpolation function.
f32 Lerp(f32 a, f32 b, f32 t);

static inline f32 Abs(f32 value)
{
    return (value < 0.0f) ? -value : value;
}

static inline f32 Sign(f32 value)
{
    return glm::sign(value);
}

static inline f32 Sin(f32 value)
{
    return glm::sin(value);
}

static inline f32 Cos(f32 value)
{
    return glm::cos(value);
}

static inline f32 Tan(f32 value)
{
    return glm::tan(value);
}

static inline f32 Radians(f32 degree)
{
    return glm::radians(degree);
}

static inline f32 Degrees(f32 radian)
{
    return glm::degrees(radian);
}

static inline f32 LengthSquared(v2 value)
{
    return value.x * value.x + value.y * value.y;
}

static inline v2 Normalize(v2 value)
{
    return glm::normalize(value);
}

static inline f32 Length(v2 value)
{
    return glm::length(value);
}

static inline v3 Normalize(v3 value)
{
    return glm::normalize(value);
}

// Creates a box2 from two points.
Box2 CreateBox2(const glm::vec2& min, const glm::vec2& max);

// Returns the closest point to the box from a point.
glm::vec2 Box2_ClosestPoint(const Box2* box, const glm::vec2& point);

// Returns true if a box2 and a circle are intersecting.
b32 Box2_CircleIntersect(const Box2* box, const glm::vec2& circleCenter, f32 circleRadius);

v2 Box2_GetCenter(const Box2* box);

// Creates a box3 from two points.
Box3 CreateBox3(glm::vec3 min, glm::vec3 max);

// Returns the closest point to the box from a point.
glm::vec3 Box3_ClosestPoint(const Box3* box, glm::vec3 point);

b32 SpheresIntersect(const glm::vec3& center1, f32 radius1, const glm::vec3& center2, f32 radius);

//b32 Box3_RayIntersect(const Box3* box, v3 rayOrigin, v3 rayDirection, f32* outDistance);

void Box3_Translate(Box3* box, v3 translation);

// Returns true if a box3 and a sphere are intersecting.
b32 Box3_SphereIntersect(const Box3* box, glm::vec3 sphereCenter, f32 sphereRadius);

// Creates a plane from three points.
void Plane_CreateFromPoints(Plane* plane, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

// Creates a plane from a normal and a point.
void Plane_CreateFromNormalAndPoint(Plane* plane, const glm::vec3& normal, const glm::vec3& point);

// Creates a plane from a normal and a distance from the origin.
void Plane_CreateFromNormalAndDistance(Plane* plane, const glm::vec3& normal, f32 distance);