#pragma once
#include "core/types.h"
#include <glm/glm.hpp>

struct Plane
{
    glm::vec3 normal;
    f32 d;
};

void Plane_CreateFromPoints(Plane* plane, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
void Plane_CreateFromNormalAndPoint(Plane* plane, const glm::vec3& normal, const glm::vec3& point);
void Plane_CreateFromNormalAndDistance(Plane* plane, const glm::vec3& normal, f32 distance);