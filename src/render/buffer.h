#pragma once
#include "core/types.h"

struct VertexBuffer;
struct IndexBuffer;

struct VertexLayout
{
    s32 attribute_count;    // Number of attributes in the layout.
    s32 attribute_sizes[4]; // Sizes of the attributes (e.g. [3, 2] for position (vec3) and uv (vec2)).
};

// Creates a new vertex buffer.
VertexBuffer* VertexBufferCreate(const void* data, s32 size);

// Destroyes the vertex buffer.
void VertexBufferDestroy(VertexBuffer* buffer);

// Bind the vertex buffer.
void VertexBufferBind(VertexBuffer* buffer);

// Unbind the vertex buffer.
void VertexBufferUnbind();

// Update the vertex buffer.
void VertexBufferUpdate(VertexBuffer* buffer, const void* data, s32 size, s32 offset = 0);

// Set the vertex layout.
void VertexBufferSetLayout(VertexBuffer* buffer, const VertexLayout* layout);

// Creates a new index buffer.
IndexBuffer* IndexBufferCreate(const u32* data, s32 size);

// Destroyes the index buffer.
void IndexBufferDestroy(IndexBuffer* buffer);

// Bind the index buffer.
void IndexBufferBind(IndexBuffer* buffer);

// Unbind the index buffer.
void IndexBufferUnbind();

// Update the index buffer.
void IndexBufferUpdate(IndexBuffer* buffer, const u32* data, s32 size, s32 offset = 0);