#pragma once
#include "core/types.h"

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
f32 Math_Lerp(f32 a, f32 b, f32 t);

// Creates a box2 from two points.
Box2 CreateBox2(const glm::vec2& min, const glm::vec2& max);

// Returns the closest point to the box from a point.
glm::vec2 Box2_ClosestPoint(const Box2* box, const glm::vec2& point);

// Returns true if a box2 and a circle are intersecting.
b32 Box2_CircleIntersect(const Box2* box, const glm::vec2& circleCenter, f32 circleRadius);

// Creates a box3 from two points.
Box3 CreateBox3(glm::vec3 min, glm::vec3 max);

// Returns the closest point to the box from a point.
glm::vec3 Box3_ClosestPoint(const Box3* box, glm::vec3 point);

// Returns true if a box3 and a sphere are intersecting.
b32 Box3_SphereIntersect(const Box3* box, glm::vec3 sphereCenter, f32 sphereRadius);

// Creates a plane from three points.
void Plane_CreateFromPoints(Plane* plane, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

// Creates a plane from a normal and a point.
void Plane_CreateFromNormalAndPoint(Plane* plane, const glm::vec3& normal, const glm::vec3& point);

// Creates a plane from a normal and a distance from the origin.
void Plane_CreateFromNormalAndDistance(Plane* plane, const glm::vec3& normal, f32 distance);