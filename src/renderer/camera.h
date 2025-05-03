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
Camera CameraCreatePerspective(f32 fov, f32 aspect, f32 near, f32 far);

// Creates a new orthographic camera.
Camera CameraCreateOrthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

// Returns the view matrix of a camera.
glm::mat4 CameraGetViewMatrix(Camera* camera);

// Returns the projection matrix of a camera.
glm::mat4 CameraGetProjectionMatrix(Camera* camera);

// Returns the forward vector of a camera.
glm::vec3 CameraGetForward(Camera* camera);

// Returns the right vector of a camera.
glm::vec3 CameraGetRight(Camera* camera);