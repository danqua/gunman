#include "camera.h"

Camera CameraCreatePerspective(f32 fov, f32 aspect, f32 near, f32 far)
{
    Camera result = {};
    result.type = CAMERA_TYPE_PERSPECTIVE;
    result.projection.perspective.fov = fov;
    result.projection.perspective.aspect = aspect;
    result.projection.perspective.near = near;
    result.projection.perspective.far = far;
    return result;
}

Camera CameraCreateOrthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    Camera result = {};
    result.type = CAMERA_TYPE_ORTHOGRAPHIC;
    result.projection.orthographic.left = left;
    result.projection.orthographic.right = right;
    result.projection.orthographic.bottom = bottom;
    result.projection.orthographic.top = top;
    result.projection.orthographic.near = near;
    result.projection.orthographic.far = far;
    return result;
}

Mat4 CameraGetViewMatrix(Camera* camera)
{
    Vec3 target = Vec3Add(camera->position, CameraGetForward(camera));
    Vec3 world_up = Vec3{ 0.0f, 1.0f, 0.0f };
    Mat4 result = Mat4LookAt(camera->position, target, world_up);
    return result;
}

Mat4 CameraGetProjectionMatrix(Camera* camera)
{
    switch (camera->type)
    {
        case CAMERA_TYPE_PERSPECTIVE:
        {
            return Mat4Perspective(camera->projection.perspective.fov,
                camera->projection.perspective.aspect,
                camera->projection.perspective.near,
                camera->projection.perspective.far);
        } break;

        case CAMERA_TYPE_ORTHOGRAPHIC:
        {
            return Mat4Orthographic(camera->projection.orthographic.left,
                camera->projection.orthographic.right,
                camera->projection.orthographic.bottom,
                camera->projection.orthographic.top,
                camera->projection.orthographic.near,
                camera->projection.orthographic.far);
        } break;
    }
    return Mat4Identity();
}

Vec3 CameraGetForward(Camera* camera)
{
    f32 pitch = DegToRad(camera->rotation.x);
    f32 yaw = DegToRad(camera->rotation.y - 90.0f);
        
    Vec3 result = {};
    result.x = Cos(yaw) * Cos(pitch);
    result.y = Sin(pitch);
    result.z = Sin(yaw) * Cos(pitch);
    return result;
}