#include "vertex_buffer.h"
#include "platform/platform.h"

#include <glad/glad.h>

struct VertexBuffer
{
    u32 id;                 // OpenGL buffer id.
    s32 size;               // Size in bytes of the vertex buffer.
    s32 vertex_count;       // Number of vertices in the buffer.
};

VertexBuffer* VertexBufferCreate(const void* data, s32 size)
{
    VertexBuffer* buffer = (VertexBuffer*)PlatformAllocateMemory(sizeof(VertexBuffer));
    buffer->size = size;

    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return buffer;
}

void VertexBufferDestroy(VertexBuffer* buffer)
{
    glDeleteBuffers(1, &buffer->id);
    PlatformFreeMemory(buffer);
}

void VertexBufferBind(VertexBuffer* buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
}

void VertexBufferUnbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferUpdate(VertexBuffer* buffer, const void* data, s32 size, s32 offset)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void VertexBufferSetLayout(VertexBuffer* buffer, const VertexLayout* layout)
{
    s32 stride = 0;
    for (s32 i = 0; i < layout->attribute_count; i++)
    {
        stride += layout->attribute_sizes[i] * sizeof(float);
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);

    u64 offset = 0;
    for (s32 i = 0; i < layout->attribute_count; i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,
            layout->attribute_sizes[i],
            GL_FLOAT,
            GL_FALSE,
            stride,
            (const void*)offset);

        offset += layout->attribute_sizes[i] * sizeof(float);
    }
}