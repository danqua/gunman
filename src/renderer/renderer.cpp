#include "renderer.h"
#include "line_renderer.h"
#include "loader.h"
#include "core/platform.h"
#include "core/assets.h"

#define MAX_LIGHTS 16
#define MAX_RENDER_COMMANDS 128

struct RenderCommand
{
    const Mesh* mesh;
    const Material* material;
    glm::mat4 transform;
};

struct RenderState
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    Light lights[MAX_LIGHTS];
    u32 lightCount;

    RenderCommand commands[MAX_RENDER_COMMANDS];
    u32 commandCount;

    ShaderId screenShader;
    VertexBufferId screenVbo;
    FramebufferId framebuffer;
};

static RenderState state;

Mesh CreateMesh(const Vertex* vertices, u32 vertexCount, const u32* indices, u32 indexCount)
{
    static BufferLayout layout = {};
    layout.count = 5;
    layout.stride = sizeof(Vertex);
    layout.elements[0] = { 0, AttribType_Float3 };
    layout.elements[1] = { 1, AttribType_Float3 };
    layout.elements[2] = { 2, AttribType_Float3 };
    layout.elements[3] = { 3, AttribType_Float2 };
    layout.elements[4] = { 4, AttribType_Float2 };
    
    Mesh mesh = {};
    mesh.vbo = RHI_CreateVertexBuffer(vertices, vertexCount * sizeof(Vertex), layout);
    mesh.ibo = RHI_CreateIndexBuffer(indices, indexCount);
    mesh.indexCount = indexCount;
    return mesh;
}

void Renderer_Init(Arena* arena)
{
    // Init framebuffer
    {
        state.framebuffer = RHI_CreateFramebuffer(320, 180);
        state.screenShader = Asset_LoadShader("shaders/screen.vs", "shaders/screen.fs");

        f32 screenVertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };

        BufferLayout screenMeshLayout = {};
        screenMeshLayout.count = 2;
        screenMeshLayout.stride = sizeof(f32) * 4;
        screenMeshLayout.elements[0] = { 0, AttribType_Float2 };
        screenMeshLayout.elements[1] = { 1, AttribType_Float2 };

        state.screenVbo = RHI_CreateVertexBuffer(screenVertices, sizeof(screenVertices), screenMeshLayout);
    }

    LineRenderer_Init(arena);
}

void Renderer_Shutdown()
{
    RHI_DestroyFramebuffer(state.framebuffer);
    LineRenderer_Shutdown();
}

void Renderer_SetSize(s32 width, s32 height)
{
    
}

void Renderer_BeginFrame(const Camera* camera)
{
    state.projectionMatrix = Camera_GetProjectionMatrix(camera);
    state.viewMatrix = Camera_GetViewMatrix(camera);

    LineRenderer_BeginFrame(camera);
}

void Renderer_EndFrame()
{
    // Main pass
    u32 framebufferWidth = RHI_GetFramebufferWidth(state.framebuffer);
    u32 framebufferHeight = RHI_GetFramebufferHeight(state.framebuffer);
    RHI_BindFramebuffer(state.framebuffer);
    RHI_SetViewport(0, 0, framebufferWidth, framebufferHeight);
    RHI_SetEnableDepthTest(true);
    RHI_ClearColor();
    RHI_ClearDepth();

    for (u32 i = 0; i < state.commandCount; ++i)
    {
        const RenderCommand* command = &state.commands[i];
        const Mesh* mesh = command->mesh;
        const Material* material = command->material;

        RHI_BindShader(material->shader);
        RHI_SetShaderUniformMat4(material->shader, "uProjectionMatrix", state.projectionMatrix);
        RHI_SetShaderUniformMat4(material->shader, "uViewMatrix", state.viewMatrix);
        RHI_SetShaderUniformMat4(material->shader, "uModelMatrix", command->transform);

        RHI_SetShaderUniformVec3(material->shader, "uDiffuseColor", material->diffuseColor);

        if (material->diffuseTexture)
        {
            RHI_SetShaderUniformInt(material->shader, "uDiffuseTexture", 0);
            RHI_BindTexture(material->diffuseTexture, 0);
        }

        RHI_SetShaderUniformInt(material->shader, "uUseLightmap", (s32)material->useLightmap);
        if (material->useLightmap)
        {
            RHI_SetShaderUniformInt(material->shader, "uLightmapTexture", 1);
            RHI_BindTexture(material->lightmapTexture, 1);
        }

        for (u32 i = 0; i < state.lightCount; ++i)
        {
            Light* light = &state.lights[i];
            RHI_SetShaderUniformVec3(material->shader, TextFormat("uLights[%d].position", i), light->position);
            RHI_SetShaderUniformVec3(material->shader, TextFormat("uLights[%d].color", i), light->color);
            RHI_SetShaderUniformFloat(material->shader, TextFormat("uLights[%d].intensity", i), light->intensity);
            RHI_SetShaderUniformFloat(material->shader, TextFormat("uLights[%d].range", i), light->range);
        }
        RHI_SetShaderUniformInt(material->shader, "uLightCount", state.lightCount);

        RHI_BindVertexBuffer(mesh->vbo);
        RHI_BindIndexBuffer(mesh->ibo);
        RHI_DrawIndexed(mesh->indexCount);
    }


    LineRenderer_EndFrame();

    RHI_UnbindFramebuffer();

    // Screen pass
    RHI_SetViewport(0, 0, Platform_GetWindowWidth(), Platform_GetWindowHeight());
    RHI_ClearColor();
    RHI_SetEnableDepthTest(false);

    RHI_BindShader(state.screenShader);
    RHI_BindVertexBuffer(state.screenVbo);
    RHI_BindTexture(RHI_GetFramebufferTexture(state.framebuffer), 0);
    RHI_SetDrawMode(DrawMode_TriangleFan);
    RHI_Draw(4);
    RHI_SetDrawMode(DrawMode_Triangles);

    RHI_SetEnableDepthTest(true);

    state.lightCount = 0;
    state.commandCount = 0;
}

void Renderer_AddLight(const Light* light)
{
    if (state.lightCount >= MAX_LIGHTS)
    {
        return;
    }

    state.lights[state.lightCount] = *light;
    state.lightCount++;
}

void Renderer_DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec4 color)
{
    LineRenderer_DrawLine(v1, v2, color);
}

void Renderer_DrawBox(glm::vec3 min, glm::vec3 max, glm::vec4 color)
{
    glm::vec3 v1 = { min.x, min.y, min.z };
    glm::vec3 v2 = { max.x, min.y, min.z };
    glm::vec3 v3 = { max.x, max.y, min.z };
    glm::vec3 v4 = { min.x, max.y, min.z };
    glm::vec3 v5 = { min.x, min.y, max.z };
    glm::vec3 v6 = { max.x, min.y, max.z };
    glm::vec3 v7 = { max.x, max.y, max.z };
    glm::vec3 v8 = { min.x, max.y, max.z };

    LineRenderer_DrawLine(v1, v2, color);
    LineRenderer_DrawLine(v2, v3, color);
    LineRenderer_DrawLine(v3, v4, color);
    LineRenderer_DrawLine(v4, v1, color);

    LineRenderer_DrawLine(v5, v6, color);
    LineRenderer_DrawLine(v6, v7, color);
    LineRenderer_DrawLine(v7, v8, color);
    LineRenderer_DrawLine(v8, v5, color);

    LineRenderer_DrawLine(v1, v5, color);
    LineRenderer_DrawLine(v2, v6, color);
    LineRenderer_DrawLine(v3, v7, color);
    LineRenderer_DrawLine(v4, v8, color);
}

void Renderer_DrawMesh(const Mesh* mesh, const Material* material, const glm::mat4& transform)
{
    if (state.commandCount >= MAX_RENDER_COMMANDS)
    {
        return;
    }

    RenderCommand* command = &state.commands[state.commandCount++];
    command->mesh = mesh;
    command->material = material;
    command->transform = transform;
}
