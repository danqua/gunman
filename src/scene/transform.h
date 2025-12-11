#pragma once
#include <glm/glm.hpp>

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

// Creates a default Transform with position (0,0,0), rotation (0,0,0), and scale (1,1,1).
Transform CreateDefaultTransform();

// Returns the transformation matrix for a given Transform.
glm::mat4 Transform_GetMatrix(const Transform* transform);

// Returns the inverse transformation matrix for a given Transform.
glm::mat4 Transform_GetMatrixInv(const Transform* transform);

// Returns the right direction vector for a given Transform.
glm::vec3 Transform_GetRight(const Transform* transform);

// Returns the forward direction vector for a given Transform.
glm::vec3 Transform_GetForward(const Transform* transform);