#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera CameraCreatePerspective(f32 fov, f32 aspect, f32 near, f32 far)
{
    Camera result = {};
    result.type = CameraType_Perspective;
    result.projection.perspective.fov = fov;
    result.projection.perspective.aspect = aspect;
    result.projection.perspective.near = near;
    result.projection.perspective.far = far;
    return result;
}

Camera CameraCreateOrthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    Camera result = {};
    result.type = CameraType_Orthographic;
    result.projection.orthographic.left = left;
    result.projection.orthographic.right = right;
    result.projection.orthographic.bottom = bottom;
    result.projection.orthographic.top = top;
    result.projection.orthographic.near = near;
    result.projection.orthographic.far = far;
    return result;
}

glm::mat4 CameraGetViewMatrix(Camera* camera)
{
    glm::vec3 target = camera->position + CameraGetForward(camera);
    glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f, 0.0f };
    glm::mat4 result = glm::lookAt(camera->position, target, worldUp);
    return result;
}

glm::mat4 CameraGetProjectionMatrix(Camera* camera)
{
    switch (camera->type)
    {
        case CameraType_Perspective:
        {
            return glm::perspective(camera->projection.perspective.fov,
                                    camera->projection.perspective.aspect,
                                    camera->projection.perspective.near,
                                    camera->projection.perspective.far);
        } break;

        case CameraType_Orthographic:
        {
            return glm::ortho(camera->projection.orthographic.left,
                              camera->projection.orthographic.right,
                              camera->projection.orthographic.bottom,
                              camera->projection.orthographic.top,
                              camera->projection.orthographic.near,
                              camera->projection.orthographic.far);
        } break;
    }
    return glm::mat4(1.0f);
}

glm::vec3 CameraGetForward(Camera* camera)
{
    f32 pitch = glm::radians(camera->rotation.x);
    f32 yaw = glm::radians(camera->rotation.y - 90.0f);
        
    glm::vec3 result = {};
    result.x = glm::cos(yaw) * glm::cos(pitch);
    result.y = glm::sin(pitch);
    result.z = glm::sin(yaw) * glm::cos(pitch);
    return result;
}

glm::vec3 CameraGetRight(Camera* camera)
{
    glm::vec3 forward = CameraGetForward(camera);
    glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f, 0.0f };
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    return right;
}
