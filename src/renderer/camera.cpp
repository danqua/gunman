#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera Camera_CreatePerspective(f32 fov, f32 aspect, f32 near, f32 far)
{
    Camera result = {};
    result.type = CameraType_Perspective;
    result.projection.perspective.fov = fov;
    result.projection.perspective.aspect = aspect;
    result.projection.perspective.near = near;
    result.projection.perspective.far = far;
    return result;
}

Camera Camera_CreateOrthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
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

glm::mat4 Camera_GetViewMatrix(const Camera* camera)
{
    glm::vec3 target = camera->position + Camera_GetForward(camera);
    glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f, 0.0f };
    glm::mat4 result = glm::lookAt(camera->position, target, worldUp);
    return result;
}

glm::mat4 Camera_GetProjectionMatrix(const Camera* camera)
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

glm::vec3 Camera_GetForward(const Camera* camera)
{
    f32 pitch = glm::radians(camera->rotation.x);
    f32 yaw = glm::radians(camera->rotation.y - 90.0f);
        
    glm::vec3 result = {};
    result.x = glm::cos(yaw) * glm::cos(pitch);
    result.y = glm::sin(pitch);
    result.z = glm::sin(yaw) * glm::cos(pitch);
    return result;
}

glm::vec3 Camera_GetRight(const Camera* camera)
{
    glm::vec3 forward = Camera_GetForward(camera);
    glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f, 0.0f };
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    return right;
}
