#pragma once
#include "core/types.h"

#include <glm/glm.hpp>

typedef u32 ShaderId;
typedef u32 VertexBufferId;
typedef u32 IndexBufferId;
typedef u32 TextureId;
typedef u32 FramebufferId;

enum BufferUsage
{
    BufferUsage_Static,
    BufferUsage_Dynamic,
    BufferUsage_Stream
};

enum AttribType
{
    AttribType_Int,
    AttribType_Uint,
    AttribType_Float,
    AttribType_Float2,
    AttribType_Float3,
    AttribType_Float4,
    AttribType_Mat3,
    AttribType_Mat4,
    AttribType_Bool
};

enum DrawMode
{
    DrawMode_Triangles,
    DrawMode_TriangleStrip,
    DrawMode_TriangleFan,
    DrawMode_Lines
};

struct BufferElement
{
    s32 location;
    AttribType type;
};

#define MAX_BUFFER_ELEMENTS 8

struct BufferLayout
{
    u32 count;
    u32 stride;
    BufferElement elements[MAX_BUFFER_ELEMENTS];
};

void RHI_Init();
void RHI_Shutdown();

void RHI_ClearColor();
void RHI_ClearDepth();
void RHI_ClearStencil();
void RHI_SetClearColor(f32 r, f32 g, f32 b, f32 a);

// Shader
ShaderId RHI_CreateShader(const char* vsSourec, const char* fsSource);
void RHI_DestroyShader(ShaderId shader);
void RHI_BindShader(ShaderId shader);
void RHI_UnbindShader();
void RHI_SetShaderUniformInt(ShaderId shader, const char* name, s32 value);
void RHI_SetShaderUniformFloat(ShaderId shader, const char* name, f32 value);
void RHI_SetShaderUniformVec2(ShaderId shader, const char* name, const glm::vec2& value);
void RHI_SetShaderUniformVec3(ShaderId shader, const char* name, const glm::vec3& value);
void RHI_SetShaderUniformVec4(ShaderId shader, const char* name, const glm::vec4& value);
void RHI_SetShaderUniformMat4(ShaderId shader, const char* name, const glm::mat4& value);

// Vertex Buffer
VertexBufferId RHI_CreateVertexBuffer(const void* data, u32 size, const BufferLayout& layout, BufferUsage usage = BufferUsage_Static);
void RHI_DestroyVertexBuffer(VertexBufferId vertexBuffer);
void RHI_BindVertexBuffer(VertexBufferId vertexBuffer);
void RHI_UpdateVertexBuffer(VertexBufferId vertexBuffer, const void* data, u32 size, u32 offset = 0);
void RHI_UnbindVertexBuffer();

// Index Buffer
IndexBufferId RHI_CreateIndexBuffer(const u32* indices, u32 count, BufferUsage usage = BufferUsage_Static);
void RHI_DestroyIndexBuffer(IndexBufferId indexBuffer);
void RHI_BindIndexBuffer(IndexBufferId indexBuffer);
void RHI_UnbindIndexBuffer();

// Texture
TextureId RHI_CreateTexture(const void* pixels, u32 width, u32 height);
void RHI_DestroyTexture(TextureId texture);
void RHI_BindTexture(TextureId texture, u32 slot = 0);
void RHI_UnbindTexture(TextureId texture);

// Framebuffer
FramebufferId RHI_CreateFramebuffer(u32 width, u32 height);
void RHI_DestroyFramebuffer(FramebufferId framebuffer);
void RHI_BindFramebuffer(FramebufferId framebuffer);
void RHI_UnbindFramebuffer();

// Draw
void RHI_SetDrawMode(DrawMode mode);
void RHI_Draw(u32 vertexCount, u32 offset = 0);
void RHI_DrawIndexed(u32 vertexCount);