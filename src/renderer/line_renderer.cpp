#include "line_renderer.h"
#include "rhi.h"
#include "core/assets.h"

#define MAX_LINES 1024

struct LineVertex
{
    glm::vec3 position;
    glm::vec4 color;
};

struct LineRendererState
{
    ShaderId shader;
    LineVertex* lines;
    u32 lineCount;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    VertexBufferId vbo;
};

static LineRendererState state;

void LineRenderer_Init(Arena* arena)
{
    u64 size = sizeof(LineVertex) * MAX_LINES;
    state.lines = ArenaPushArray(arena, LineVertex, MAX_LINES);
    state.shader = Asset_LoadShader("shaders/line.vs", "shaders/line.fs");

    BufferLayout layout = {};
    layout.count = 2;
    layout.stride = sizeof(LineVertex);
    layout.elements[0] = { 0, AttribType_Float3 };
    layout.elements[1] = { 1, AttribType_Float4 };

    state.vbo = RHI_CreateVertexBuffer(nullptr, sizeof(LineVertex) * MAX_LINES, layout, BufferUsage_Dynamic);
}

void LineRenderer_Shutdown()
{
    state.lines = nullptr;
    state.lineCount = 0;
}

void LineRenderer_BeginFrame(const Camera* camera)
{
    state.projectionMatrix = Camera_GetProjectionMatrix(camera);
    state.viewMatrix = Camera_GetViewMatrix(camera);
    state.lineCount = 0;
}

void LineRenderer_EndFrame()
{
    RHI_SetEnableDepthTest(true);

    if (state.lineCount > 0)
    {
        RHI_BindShader(state.shader);
        RHI_SetShaderUniformMat4(state.shader, "uProjectionMatrix", state.projectionMatrix);
        RHI_SetShaderUniformMat4(state.shader, "uViewMatrix", state.viewMatrix);

        RHI_BindVertexBuffer(state.vbo);
        RHI_UpdateVertexBuffer(state.vbo, state.lines, sizeof(LineVertex) * state.lineCount);

        RHI_SetDrawMode(DrawMode_Lines);
        RHI_Draw(state.lineCount);
        RHI_SetDrawMode(DrawMode_Triangles);
    }
    RHI_SetEnableDepthTest(false);
}

void LineRenderer_DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec4 color)
{
    if (state.lineCount + 2 > MAX_LINES)
    {
        return;
    }

    LineVertex* vertices = &state.lines[state.lineCount];
    vertices[0].position = v1;
    vertices[0].color = color;
    vertices[1].position = v2;
    vertices[1].color = color;
    state.lineCount += 2;
}
