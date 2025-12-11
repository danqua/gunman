#pragma once
#include "core/types.h"
#include "renderer/camera.h"
#include "transform.h"

struct CameraController
{
    f32 speed;
    f32 speedMod;
    f32 friction;
    f32 rotationSpeed;
    glm::vec3 velocity;
};

CameraController CreateDefaultCameraController();

void UpdateCameraControls(CameraController* controller, Transform* transform, f32 deltaTime);