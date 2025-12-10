#include "math.h"

f32 Lerp(f32 a, f32 b, f32 t)
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

v2 Box2_GetCenter(const Box2* box)
{
    v2 center;
    center.x = (box->min.x + box->max.x) * 0.5f;
    center.y = (box->min.y + box->max.y) * 0.5f;

    return center;
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

b32 SpheresIntersect(const glm::vec3& center1, f32 radius1, const glm::vec3& center2, f32 radius)
{
    glm::vec3 distance = center1 - center2;
    f32 radiusSum = radius1 + radius;
    return (glm::dot(distance, distance) < (radiusSum * radiusSum));
}

b32 Box3_RayIntersect(const Box3* box, v3 rayOrigin, v3 rayDirection, f32* outDistance)
{
    const f32 tMinInit = -FLT_MAX;
    const f32 tMaxInit = FLT_MAX;

    f32 tMin = tMinInit;
    f32 tMax = tMaxInit;

    // for each axis
    for (s32 i = 0; i < 3; ++i)
    {
        f32 origin = rayOrigin[i];
        f32 direction = rayDirection[i];
        f32 bMin = box->min[i];
        f32 bMax = box->max[i];

        if (Abs(direction) < 1e-8f)
        {
            if (origin < bMin || origin > bMax)
                return false;
        }
        else
        {
            f32 invD = 1.0f / direction;
            f32 t1 = (bMin - origin) * invD;
            f32 t2 = (bMax - origin) * invD;

            if (t1 > t2) std::swap(t1, t2);

            tMin = glm::max(tMin, t1);
            tMax = glm::min(tMax, t2);

            if (tMin > tMax)
                return false;
        }
    }

    if (tMax < 0)
        return false;

    if (outDistance)
        *outDistance = (tMin >= 0) ? tMin : tMax;

    return true;
}

b32 Box3_SphereIntersect(const Box3* box, glm::vec3 sphereCenter, f32 sphereRadius)
{
    glm::vec3 closestPoint = Box3_ClosestPoint(box, sphereCenter);
    glm::vec3 distance = sphereCenter - closestPoint;
    return (glm::dot(distance, distance) < (sphereRadius * sphereRadius));
}

void Box3_Translate(Box3* box, v3 translation)
{
    box->min += translation;
    box->max += translation;
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