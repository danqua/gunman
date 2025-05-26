#include "math.h"

f32 Math_Lerp(f32 a, f32 b, f32 t)
{
    return a + t * (b - a);
}

Box2 CreateBox2(const glm::vec2& min, const glm::vec2& max)
{
    Box2 box;
    box.min = min;
    box.max = max;

    return box;
}

glm::vec2 Box2_ClosestPoint(const Box2* box, const glm::vec2& point)
{
    glm::vec2 closestPoint;
    closestPoint.x = glm::clamp(point.x, box->min.x, box->max.x);
    closestPoint.y = glm::clamp(point.y, box->min.y, box->max.y);

    return closestPoint;
}

b32 Box2_CircleIntersect(const Box2* box, const glm::vec2& circleCenter, f32 circleRadius)
{
    glm::vec2 closestPoint = Box2_ClosestPoint(box, circleCenter);
    glm::vec2 distance = circleCenter - closestPoint;
    return (glm::dot(distance, distance) < (circleRadius * circleRadius));
}

Box3 CreateBox3(glm::vec3 min, glm::vec3 max)
{
    Box3 box;
    box.min = min;
    box.max = max;

    return box;
}

glm::vec3 Box3_ClosestPoint(const Box3* box, glm::vec3 point)
{
    glm::vec3 closestPoint;
    closestPoint.x = glm::clamp(point.x, box->min.x, box->max.x);
    closestPoint.y = glm::clamp(point.y, box->min.y, box->max.y);
    closestPoint.z = glm::clamp(point.z, box->min.z, box->max.z);

    return closestPoint;
}

b32 Box3_SphereIntersect(const Box3* box, glm::vec3 sphereCenter, f32 sphereRadius)
{
    glm::vec3 closestPoint = Box3_ClosestPoint(box, sphereCenter);
    glm::vec3 distance = sphereCenter - closestPoint;
    return (glm::dot(distance, distance) < (sphereRadius * sphereRadius));
}


void Plane_CreateFromPoints(Plane* plane, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    const glm::vec3& v1 = p2 - p1;
    const glm::vec3& v2 = p3 - p1;
    plane->normal = glm::normalize(glm::cross(v1, v2));
    plane->d = -glm::dot(plane->normal, p1);
}

void Plane_CreateFromNormalAndPoint(Plane* plane, const glm::vec3& normal, const glm::vec3& point)
{
    plane->normal = glm::normalize(normal);
    plane->d = -glm::dot(plane->normal, point);
}

void Plane_CreateFromNormalAndDistance(Plane* plane, const glm::vec3& normal, f32 distance)
{
    plane->normal = glm::normalize(normal);
    plane->d = distance;
}