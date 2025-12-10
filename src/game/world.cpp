#include "world.h"
#include <glm/gtc/quaternion.hpp>

v3 TransformComponent::GetForward() const
{
    v3 forward = glm::quat(glm::radians(rotation)) * v3(0, 0, -1);
    return forward;
}

v3 TransformComponent::GetRight() const
{
    v3 right = glm::quat(glm::radians(rotation)) * v3(1, 0, 0);
    return right;
}

v3 TransformComponent::GetUp() const
{
    v3 up = glm::quat(glm::radians(rotation)) * v3(0, 1, 0);
    return up;
}

m4x4 TransformComponent::GetTransformMatrix() const
{
    m4x4 transform = GetTransformMatrixNoScale();
    transform = glm::scale(transform, scale);
    return transform;
}

m4x4 TransformComponent::GetTransformMatrixNoScale() const
{
    m4x4 transform = glm::translate(m4x4(1.0f), position);
    transform = glm::rotate(transform, glm::radians(rotation.y), v3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.x), v3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z), v3(0, 0, 1));
    return transform;
}