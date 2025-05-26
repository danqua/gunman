#include "renderer_2d.h"
#include "rhi.h"
#include "camera.h"
#include "core/platform.h"


#define MAX_LINE_VERTICES 1024

struct LineVertex
{
    glm::vec3 position;
    glm::vec4 color;
};

struct Renderer2D
{
    Camera camera;
    LineVertex* vertices;
    u32 vertexCount;
    VertexBufferId vertexBuffer;
    ShaderId shader;
    f32 zOrder;
};

static Renderer2D renderer;

void Renderer2D_Init(Arena* arena)
{
    f32 width = (f32)Platform_GetWindowWidth();
    f32 height = (f32)Platform_GetWindowHeight();

    renderer.camera = Camera_CreateOrthographic(0.0f, width, height,0.0f, -1.0f, 1.0f);
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

void Renderer2D_Shutdown()
{
    RHI_DestroyVertexBuffer(renderer.vertexBuffer);
    RHI_DestroyShader(renderer.shader);
    renderer.vertices = nullptr;
    renderer.vertexCount = 0;
    renderer.vertexBuffer = 0;
    renderer.shader = 0;
}

void Renderer2D_BeginFrame()
{
    renderer.vertexCount = 0;
    renderer.zOrder = 0;
}

void Renderer2D_EndFrame()
{
    Renderer2D_Flush();
}

void Renderer2D_Flush()
{
    if (renderer.vertexCount == 0)
    {
        return;
    }

    RHI_SetDrawMode(DrawMode_Lines);
    RHI_BindShader(renderer.shader);
    RHI_BindVertexBuffer(renderer.vertexBuffer);
    RHI_UpdateVertexBuffer(renderer.vertexBuffer, renderer.vertices, sizeof(LineVertex) * renderer.vertexCount);
    RHI_SetShaderUniformMat4(renderer.shader, "uProjectionMatrix", Camera_GetProjectionMatrix(&renderer.camera));
    RHI_SetShaderUniformMat4(renderer.shader, "uViewMatrix", Camera_GetViewMatrix(&renderer.camera));
    RHI_Draw(renderer.vertexCount);
}

void Renderer2D_DrawLine(const glm::vec2& start, const glm::vec2& end, Color color)
{
    if (renderer.vertexCount >= MAX_LINE_VERTICES)
    {
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

void Renderer2D_DrawRect(const glm::vec2& position, const glm::vec2& size, Color color)
{
    Renderer2D_DrawLine(position, glm::vec2(position.x + size.x, position.y), color);
    Renderer2D_DrawLine(glm::vec2(position.x + size.x, position.y), glm::vec2(position.x + size.x, position.y + size.y), color);
    Renderer2D_DrawLine(glm::vec2(position.x + size.x, position.y + size.y), glm::vec2(position.x, position.y + size.y), color);
    Renderer2D_DrawLine(glm::vec2(position.x, position.y + size.y), position, color);
}