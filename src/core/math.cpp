#include "math.h"

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