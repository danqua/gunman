#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "core/math.h"
#include "renderer/rhi.h"
#include "renderer/camera.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord0;
    glm::vec2 texCoord1;
};

struct Mesh
{
    VertexBufferId vbo;
    IndexBufferId ibo;
    u32 indexCount;
};

struct Material
{
    ShaderId shader;
    TextureId diffuseTexture;
    TextureId lightmapTexture;
    glm::vec3 diffuseColor;

    b32 useLightmap;
};

struct Light
{
    glm::vec3 position;
    glm::vec3 color;
    f32 intensity;
    f32 range;
};

Mesh CreateMesh(const Vertex* vertices, u32 vertexCount, const u32* indices, u32 indexCount);


void Renderer_Init(Arena* arena);
void Renderer_Shutdown();
void Renderer_SetSize(s32 width, s32 height);
void Renderer_BeginFrame(const Camera* camera);
void Renderer_EndFrame();
void Renderer_AddLight(const Light* light);
void Renderer_DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
void Renderer_DrawBox(glm::vec3 min, glm::vec3 max, glm::vec4 color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));
void Renderer_DrawMesh(const Mesh* mesh, const Material* material, const glm::mat4& transform);
