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

void CameraController_Update(CameraController* controller, Camera* camera, f32 deltaTime)
{
    /*
    glm::vec3 forward = Camera_GetForward(camera);
    glm::vec3 right = Camera_GetRight(camera);

    f32 speed = controller->speed;

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
        controller->velocity.z += speed;
    }
    if (IsKeyDown(Key_Q))
    {
        controller->velocity.z -= speed;
    }
    if (IsKeyDown(Key_Left))
    {
        camera->transform.rotation.z -= controller->rotationSpeed * deltaTime;
    }
    if (IsKeyDown(Key_Right))
    {
        camera->rotation.z += controller->rotationSpeed * deltaTime;
    }

    glm::vec2 mouseDelta = GetRelativeMousePosition();

    camera->position += controller->velocity * deltaTime;
    camera->rotation.x -= mouseDelta.y * game->settings.mouseSensitivity;
    camera->rotation.z += mouseDelta.x * game->settings.mouseSensitivity;
    camera->rotation.x = glm::clamp(camera->rotation.x, -89.0f, 89.0f);

    controller->velocity *= controller->friction;

    if (Abs(controller->velocity.x) < 1e-4f) controller->velocity.x = 0.0f;
    if (Abs(controller->velocity.y) < 1e-4f) controller->velocity.y = 0.0f;
    if (Abs(controller->velocity.z) < 1e-4f) controller->velocity.z = 0.0f;
    */
}
