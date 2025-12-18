#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "core/math.h"
#include "renderer/rhi.h"
#include "renderer/camera.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord0;
    glm::vec2 texCoord1;
};

struct Mesh {
    VertexBufferId vbo;
    IndexBufferId ibo;
    u32 indexCount;
    Box3 aabb;
};

struct Material {
    ShaderId shader;
    TextureId diffuseTexture;
    TextureId lightmapTexture;
    glm::vec3 diffuseColor;

    bool backfaceCulling;
    bool useLightmap;
};

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    f32 intensity;
    f32 range;
};

Mesh CreateMesh(const Vertex* vertices, u32 vertexCount, const u32* indices, u32 indexCount);
void DestroyMesh(Mesh* mesh);

void Renderer_Init(Arena* arena);
void Renderer_Shutdown();
void Renderer_SetSize(s32 width, s32 height);
void Renderer_BeginFrame(const glm::mat4& projection, const glm::mat4& view);
void Renderer_EndFrame();
void Renderer_AddLight(const Light* light);
void Renderer_DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
void Renderer_DrawBox(glm::vec3 min, glm::vec3 max, glm::vec4 color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));
void Renderer_DrawMesh(const Mesh* mesh, const Material* material, const glm::mat4& transform);
void Renderer_DrawCircle(const glm::vec3& center, f32 radius, const glm::vec3& normal, u32 segments = 32, const glm::vec4& color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));
void Renderer_DrawSphere(const glm::vec3& center, f32 radius, const glm::vec4& color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));
