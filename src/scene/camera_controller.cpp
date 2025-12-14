#include "camera_controller.h"
#include "core/input.h"
#include "game/game.h"

CameraController CreateDefaultCameraController()
{
    CameraController controller = {};
    controller.speed = 0.75f;
    controller.speedMod = 2.0f;
    controller.friction = 0.85f;
    controller.rotationSpeed = 90.0f;
    return controller;
}

void UpdateCameraControls(CameraController* controller, Transform* transform, f32 deltaTime)
{
    glm::vec3 forward = Transform_GetForward(transform);
    glm::vec3 right = Transform_GetRight(transform);

    f32 speed = controller->speed;
    f32 mouseSensitivity = 0.1f;

    if (IsKeyDown(Key_LeftShift))
    {
        speed *= controller->speedMod;
    }

    if (IsKeyDown(Key_W) || IsKeyDown(Key_Up))
    {
        controller->velocity += forward * speed;
    }
    if (IsKeyDown(Key_S) || IsKeyDown(Key_Down))
    {
        controller->velocity -= forward * speed;
    }
    if (IsKeyDown(Key_A))
    {
        controller->velocity -= right * speed;
    }
    if (IsKeyDown(Key_D))
    {
        controller->velocity += right * speed;
    }
    if (IsKeyDown(Key_E))
    {
        controller->velocity.y += speed;
    }
    if (IsKeyDown(Key_Q))
    {
        controller->velocity.y -= speed;
    }

    glm::vec2 mouseDelta = GetDeltaMousePosition();

    transform->position += controller->velocity * deltaTime;

    transform->rotation.x -= mouseDelta.y * mouseSensitivity;
    transform->rotation.y -= mouseDelta.x * mouseSensitivity;
    transform->rotation.x = glm::clamp(transform->rotation.x, -89.0f, 89.0f);

    controller->velocity *= controller->friction;

    if (Abs(controller->velocity.x) < 1e-4f) controller->velocity.x = 0.0f;
    if (Abs(controller->velocity.y) < 1e-4f) controller->velocity.y = 0.0f;
    if (Abs(controller->velocity.z) < 1e-4f) controller->velocity.z = 0.0f;
}
