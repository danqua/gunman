#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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