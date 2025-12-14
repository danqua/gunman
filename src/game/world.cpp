#include "world.h"
#include <glm/gtc/quaternion.hpp>

glm::vec3 TransformComponent::GetForward() const
{
    glm::vec3 forward = glm::quat(glm::radians(rotation)) * glm::vec3(0, 0, -1);
    return forward;
}

glm::vec3 TransformComponent::GetRight() const
{
    glm::vec3 right = glm::quat(glm::radians(rotation)) * glm::vec3(1, 0, 0);
    return right;
}

glm::vec3 TransformComponent::GetUp() const
{
    glm::vec3 up = glm::quat(glm::radians(rotation)) * glm::vec3(0, 1, 0);
    return up;
}

glm::mat4 TransformComponent::GetTransformMatrix() const
{
    glm::mat4 transform = GetTransformMatrixNoScale();
    transform = glm::scale(transform, scale);
    return transform;
}

glm::mat4 TransformComponent::GetTransformMatrixNoScale() const
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    return transform;
}