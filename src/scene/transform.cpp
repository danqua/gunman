#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Transform CreateDefaultTransform() {
    Transform result;
    result.position = glm::vec3(0.0f);
    result.rotation = glm::vec3(0.0f);
    result.scale = glm::vec3(1.0f);
    return result;
}

glm::mat4 Transform_GetMatrix(const Transform* transform) {
    glm::mat4 result = glm::translate(glm::mat4(1.0f), transform->position);
    result = glm::rotate(result, glm::radians(transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    result = glm::rotate(result, glm::radians(transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    result = glm::rotate(result, glm::radians(transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    result = glm::scale(result, transform->scale);
    return result;
}

glm::mat4 Transform_GetMatrixInv(const Transform* transform) {
    glm::mat4 result = Transform_GetMatrix(transform);
    result = glm::inverse(result);
    return result;
}

glm::vec3 Transform_GetRight(const Transform* transform) {
    glm::vec3 result = glm::quat(glm::radians(transform->rotation)) * glm::vec3(1.0f, 0.0f, 0.0f);
    result = glm::normalize(result);
    return result;
}

glm::vec3 Transform_GetForward(const Transform* transform) {
    glm::vec3 result = glm::quat(glm::radians(transform->rotation)) * glm::vec3(0.0f, 0.0f, -1.0f);
    result = glm::normalize(result);
    return result;
}