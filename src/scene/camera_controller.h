#pragma once
#include "core/types.h"
#include "renderer/camera.h"

struct CameraController
{
    f32 speed;
    f32 speedMod;
    f32 friction;
    f32 mouseSensitivity;
    f32 rotationSpeed;
    glm::vec3 velocity;
};

CameraController CreateDefaultCameraController();

void CameraController_Update(CameraController* controller, Camera* camera, f32 deltaTime);