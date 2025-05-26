#pragma once
#include "core/types.h"
#include <glm/glm.hpp>

enum CameraType
{
    CameraType_Perspective,
    CameraType_Orthographic
};

struct PerspectiveCamera
{
    f32 fov;        // Field of view in degrees.
    f32 aspect;     // Aspect ratio (width / height).
    f32 near;       // Near clipping plane.
    f32 far;        // Far clipping plane.
};

struct OrthographicCamera
{
    f32 left;       // Left clipping plane.
    f32 right;      // Right clipping plane.
    f32 bottom;     // Bottom clipping plane.
    f32 top;        // Top clipping plane.
    f32 near;       // Near clipping plane.
    f32 far;        // Far clipping plane.
};

union CameraProjection
{
    PerspectiveCamera perspective;
    OrthographicCamera orthographic;
};

struct Camera
{
    CameraType type;                // The camera type (perspective or orthographic).
    glm::vec3 position;             // The camera position.
    glm::vec3 rotation;             // The camera rotation.
    CameraProjection projection;    // The camera projection settings.
};

// Creates a new perspective camera.
Camera Camera_CreatePerspective(f32 fov, f32 aspect, f32 near, f32 far);

// Creates a new orthographic camera.
Camera Camera_CreateOrthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

// Returns the view matrix of a camera.
glm::mat4 Camera_GetViewMatrix(const Camera* camera);

// Returns the projection matrix of a camera.
glm::mat4 Camera_GetProjectionMatrix(const Camera* camera);

// Returns the forward vector of a camera.
glm::vec3 Camera_GetForward(const Camera* camera);

// Returns the right vector of a camera.
glm::vec3 Camera_GetRight(const Camera* camera);