#include "core/platform.h"
#include "core/audio.h"
#include "core/input.h"
#include "core/math.h"
#include "core/memory.h"
#include "core/containers.h"
#include "renderer/rhi.h"
#include "renderer/camera.h"
#include "renderer/image.h"
#include "renderer/renderer_2d.h"
#include "renderer/renderer.h"
#include "renderer/sprite.h"

#include "scene/camera_controller.h"
#include "scene/transform.h"

struct Ray {
    glm::vec2 origin;
    glm::vec2 direction;
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

b32 AABB_Intersects(const AABB* a, const AABB* b)
{
    return (a->min.x <= b->max.x && a->max.x >= b->min.x) &&
           (a->min.y <= b->max.y && a->max.y >= b->min.y) &&
           (a->min.z <= b->max.z && a->max.z >= b->min.z);
}




int main(int argc, char** argv)
{
    Platform_InitWindow("Gunman", 1920, 1080);
    //Platform_PlayAudioClip(audio, true);

    void* memory = Platform_Alloc(Megabytes(8));
    Platform_Assert(memory, "Failed to allocate memory.");

    Arena permanentStorage = {};
    Arena_Init(&permanentStorage, Megabytes(8), memory);
  
    RHI_Init();
    Renderer_Init(&permanentStorage);
    Renderer_SetSize(1920, 1080);
    
    Audio_Init(&permanentStorage);


    Camera camera = Camera_CreatePerspective(70.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    Transform cameraTransform = CreateDefaultTransform();
    cameraTransform.position = glm::vec3(0.0f, 0.0f, 4.0f);
    CameraController controls = CreateDefaultCameraController();
    
    while (!Platform_WindowShouldClose())
    {
        Platform_PollEvents();
        Audio_Update();

        if (IsKeyPressed(Key_Escape))
        {
            Platform_CloseWindow();
        }

        static f64 lastTime = Platform_GetTime();
        f64 currentTime = Platform_GetTime();
        f32 deltaTime = (f32)(currentTime - lastTime);
        lastTime = currentTime;

        glm::mat4 projection = Camera_GetProjectionMatrix(&camera);
        glm::mat4 view = Transform_GetMatrixInv(&cameraTransform);

        UpdateCameraControls(&controls, &cameraTransform, deltaTime);

        Renderer_BeginFrame(projection, view);
        Renderer_DrawBox(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), Color_ConvertToVec4(COLOR_WHITE));
        Renderer_EndFrame();
        Platform_SwapBuffers();
        Input_NextFrame();
    }

    RHI_Shutdown();
    Audio_Shutdown();
    return 0;
}