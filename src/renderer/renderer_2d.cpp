#include "renderer_2d.h"
#include "rhi.h"
#include "camera.h"
#include "core/platform.h"

#include <glm/gtc/matrix_transform.hpp>


Camera2D CreateDefaultCamera2D(s32 viewportWidth, s32 viewportHeight) {
    Camera2D camera = {};
    camera.pixelsPerUnit = 32.0f;
    camera.minPixelsPerUnit = 8.0f;
    camera.maxPixelsPerUnit = 128.0f;
    camera.viewportSize = glm::ivec2(viewportWidth, viewportHeight);
    return camera;
}

void Camera2D_SetViewport(Camera2D* camera, s32 viewportWidth, s32 viewportHeight) {
    camera->viewportSize = glm::ivec2(viewportWidth, viewportHeight);
}

void Camera2D_SetZoomLimits(Camera2D* camera, f32 minPixelsPerUnit, f32 maxPixelsPerUnit) {
    camera->minPixelsPerUnit = minPixelsPerUnit;
    camera->maxPixelsPerUnit = maxPixelsPerUnit;
    camera->pixelsPerUnit = glm::clamp(camera->pixelsPerUnit, minPixelsPerUnit, maxPixelsPerUnit);
}

glm::vec2 Camera2D_ScreenToWorld(const Camera2D* camera, glm::vec2 screenPos) {
    glm::vec2 halfViewport = glm::vec2(camera->viewportSize) * 0.5f;
    glm::vec2 relativePos = screenPos - halfViewport;
    glm::vec2 worldPos = camera->center + (relativePos / camera->pixelsPerUnit);
    return worldPos;
}

glm::vec2 Camera2D_WorldToScreen(const Camera2D* camera, glm::vec2 worldPos) {
    glm::vec2 halfViewport = glm::vec2(camera->viewportSize) * 0.5f;
    glm::vec2 worldDelta = worldPos - camera->center;
    glm::vec2 screenPos = worldDelta * camera->pixelsPerUnit + halfViewport;
    return screenPos;
}

void Camera2D_ZoomAtWorldPoint(Camera2D* camera, f32 zoomFactor, glm::vec2 anchorPoint) {
    f32 oldPixelsPerUnit = camera->pixelsPerUnit;
    f32 newPixelsPerUnit = glm::clamp(oldPixelsPerUnit * zoomFactor, camera->minPixelsPerUnit, camera->maxPixelsPerUnit);
    f32 scale = oldPixelsPerUnit / newPixelsPerUnit;

    glm::vec2 worldPosBeforeZoom = Camera2D_ScreenToWorld(camera, anchorPoint);
    camera->pixelsPerUnit = newPixelsPerUnit;

    glm::vec2 worldPosAfterZoom = Camera2D_ScreenToWorld(camera, anchorPoint);
    camera->center += worldPosBeforeZoom - worldPosAfterZoom;
}

glm::mat4 Camera2D_GetViewMatrix(const Camera2D* camera) {
    glm::mat4 view = glm::scale(glm::mat4(1.0f), glm::vec3(camera->pixelsPerUnit, camera->pixelsPerUnit, 1.0f));
    view = glm::translate(view, glm::vec3(-camera->center.x, -camera->center.y, 0.0f));
    return view;
}

#define MAX_LINE_VERTICES 1024

struct LineVertex {
    glm::vec3 position;
    glm::vec4 color;
};

struct Renderer2D {
    Camera camera;





    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;


    glm::vec2 viewOffset;
    LineVertex* vertices;
    u32 vertexCount;
    VertexBufferId vertexBuffer;
    ShaderId shader;
    f32 zOrder;
};

static Renderer2D renderer;

void Renderer2D_Init(Arena* arena) {
    f32 width = (f32)Platform_GetWindowWidth();
    f32 height = (f32)Platform_GetWindowHeight();

    renderer.camera = Camera_CreateOrthographic(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    renderer.viewOffset = glm::vec2(0.0f);
    renderer.zOrder = 0.0f;
    renderer.vertices = (LineVertex*)Arena_PushSize(arena, sizeof(LineVertex) * MAX_LINE_VERTICES);
    renderer.vertexCount = 0;

    BufferLayout layout = {};
    layout.count = 2;
    layout.stride = sizeof(LineVertex);
    layout.elements[0] = { 0, AttribType_Float3 };
    layout.elements[1] = { 1, AttribType_Float4 };

    renderer.vertexBuffer = RHI_CreateVertexBuffer(nullptr, sizeof(LineVertex) * MAX_LINE_VERTICES, layout, BufferUsage_Dynamic);
    renderer.shader = RHI_CreateShader(R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec4 color;

        out vec4 vColor;

        uniform mat4 uProjectionMatrix;
        uniform mat4 uViewMatrix;

        void main()
        {
            gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0);
            vColor = color;
        }
    )", R"(
        #version 330 core
        in vec4 vColor;
        out vec4 color;
        void main()
        {
            color = vColor;
        }
    )");
}

void Renderer2D_Shutdown() {
    RHI_DestroyVertexBuffer(renderer.vertexBuffer);
    RHI_DestroyShader(renderer.shader);
    renderer.vertices = nullptr;
    renderer.vertexCount = 0;
    renderer.vertexBuffer = 0;
    renderer.shader = 0;
}

void Renderer2D_SetSize(f32 width, f32 height) {
    renderer.camera = Camera_CreateOrthographic(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

void Renderer2D_SetViewOffset(f32 offsetX, f32 offsetY) {
    renderer.viewOffset = glm::vec2(offsetX, offsetY);
}

void Renderer2D_BeginFrame(const glm::mat4* projectionMatrix, const glm::mat4* viewMatrix) {
    renderer.vertexCount = 0;
    renderer.zOrder = 0;

    renderer.projectionMatrix = *projectionMatrix;
    renderer.viewMatrix = *viewMatrix;
}

void Renderer2D_EndFrame() {
    Renderer2D_Flush();

    RHI_ClearColor();
    RHI_SetDrawMode(DrawMode_Lines);
    RHI_SetEnableDepthTest(false);
    RHI_BindShader(renderer.shader);
    // RHI_SetShaderUniformMat4(renderer.shader, "uProjectionMatrix", Camera_GetProjectionMatrix(&renderer.camera));
    // RHI_SetShaderUniformMat4(renderer.shader, "uViewMatrix", glm::lookAt(glm::vec3(renderer.viewOffset, 0.0f), glm::vec3(renderer.viewOffset, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    RHI_SetShaderUniformMat4(renderer.shader, "uProjectionMatrix", renderer.projectionMatrix);
    RHI_SetShaderUniformMat4(renderer.shader, "uViewMatrix", renderer.viewMatrix);
    RHI_Draw(renderer.vertexCount);
    RHI_SetEnableDepthTest(true);
    RHI_SetDrawMode(DrawMode_Triangles);
}

void Renderer2D_Flush() {
    if (renderer.vertexCount == 0) {
        return;
    }

    RHI_BindVertexBuffer(renderer.vertexBuffer);
    RHI_UpdateVertexBuffer(renderer.vertexBuffer, renderer.vertices, sizeof(LineVertex) * renderer.vertexCount);
}

void Renderer2D_DrawLine(const glm::vec2& start, const glm::vec2& end, Color color) {
    if (renderer.vertexCount >= MAX_LINE_VERTICES) {
        Renderer2D_Flush();
    }

    LineVertex* v1 = &renderer.vertices[renderer.vertexCount++];
    LineVertex* v2 = &renderer.vertices[renderer.vertexCount++];

    v1->position = glm::vec3(start.x, start.y, renderer.zOrder);
    v1->color = glm::vec4(color.r, color.g, color.b, color.a) / 255.0f;
    v2->position = glm::vec3(end.x, end.y, renderer.zOrder);
    v2->color = glm::vec4(color.r, color.g, color.b, color.a) / 255.0f;
    renderer.zOrder += 0.001f;
}

void Renderer2D_DrawRect(const glm::vec2& position, const glm::vec2& size, Color color) {
    Renderer2D_DrawLine(position, glm::vec2(position.x + size.x, position.y), color);
    Renderer2D_DrawLine(glm::vec2(position.x + size.x, position.y), glm::vec2(position.x + size.x, position.y + size.y), color);
    Renderer2D_DrawLine(glm::vec2(position.x + size.x, position.y + size.y), glm::vec2(position.x, position.y + size.y), color);
    Renderer2D_DrawLine(glm::vec2(position.x, position.y + size.y), position, color);
}