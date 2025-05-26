#include "rhi.h"
#include "core/platform.h"
#include <glad/glad.h>

#define MAX_SHADERS 32
#define MAX_VERTEX_BUFFERS 64
#define MAX_INDEX_BUFFERS 64
#define MAX_TEXTURES 256
#define MAX_FRAMEBUFFERS 16

struct Shader
{
    u32 id;
};

struct Texture
{
    u32 id;
    u32 width;
    u32 height;
};

struct VertexBuffer
{
    u32 id;
    u32 vertexArray;
    u32 size;
    u32 usage;
};

struct IndexBuffer
{
    u32 id;
    u32 size;
    u32 usage;
};

struct Framebuffer
{
    u32 id;
    s32 width;
    s32 height;
    TextureId colorAttachment;
    TextureId depthAttachment;
};

static Shader shaders[MAX_SHADERS];
static b32 shaderUsed[MAX_SHADERS];

static Texture textures[MAX_TEXTURES];
static b32 textureUsed[MAX_TEXTURES];

static VertexBuffer vertexBuffers[MAX_VERTEX_BUFFERS];
static b32 vertexBufferUsed[MAX_VERTEX_BUFFERS];

static IndexBuffer indexBuffers[MAX_INDEX_BUFFERS];
static b32 indexBufferUsed[MAX_INDEX_BUFFERS];

static Framebuffer framebuffers[MAX_FRAMEBUFFERS];
static b32 framebufferUsed[MAX_FRAMEBUFFERS];

static GLenum drawMode;

static GLuint OpenGLCreateShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint infoLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

        if (infoLength > 0)
        {
            char* info = new char[infoLength];
            glGetShaderInfoLog(shader, infoLength, nullptr, info);
            Platform_LogWarning("Shader compilation error: %s\n", info);
            delete[] info;
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint OpenGLCreateProgram(uint32_t vertexShader, uint32_t fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint infoLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);

        if (infoLength > 0)
        {
            char* info = new char[infoLength];
            glGetProgramInfoLog(program, infoLength, nullptr, info);
            Platform_LogWarning("Shader linking error: %s\n", info);
            delete[] info;
        }

        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

static GLenum BufferUsageToOpenGL(BufferUsage usage)
{
    switch (usage)
    {
        case BufferUsage_Static: return GL_STATIC_DRAW;
        case BufferUsage_Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage_Stream: return GL_STREAM_DRAW;
        default: return GL_STATIC_DRAW;
    }
}

static GLenum AttribTypToOpenGL(AttribType type)
{
    switch (type)
    {
        case AttribType_Float: return GL_FLOAT;
        case AttribType_Float2: return GL_FLOAT;
        case AttribType_Float3: return GL_FLOAT;
        case AttribType_Float4: return GL_FLOAT;
        case AttribType_Int: return GL_INT;
        case AttribType_Uint: return GL_UNSIGNED_INT;
        case AttribType_Bool: return GL_BOOL;
        default: return GL_FLOAT;
    }
}

static GLint AttribTypeToSize(AttribType type)
{
    switch (type)
    {
        case AttribType_Float: return 1;
        case AttribType_Float2: return 2;
        case AttribType_Float3: return 3;
        case AttribType_Float4: return 4;
        case AttribType_Int: return 1;
        case AttribType_Uint: return 1;
        case AttribType_Bool: return 1;
        default: return 0;
    }
}

static GLenum TextureFilterToOpenGL(TextureFilter filter)
{
    switch (filter)
    {
        case TextureFilter_Nearest: return GL_NEAREST;
        case TextureFilter_Linear: return GL_LINEAR;
        default: return GL_LINEAR;
    }
}

void RHI_Init()
{
    gladLoadGL();

    drawMode = GL_TRIANGLES;

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

void RHI_Shutdown()
{
}

void RHI_ClearColor()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void RHI_ClearDepth()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RHI_ClearStencil()
{
    glClear(GL_STENCIL_BUFFER_BIT);
}

void RHI_SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
    glClearColor(r, g, b, a);
}

void RHI_SetViewport(u32 x, u32 y, u32 width, u32 height)
{
    glViewport(x, y, width, height);
}

void RHI_SetEnableDepthTest(bool enable)
{
    (enable ? glEnable : glDisable)(GL_DEPTH_TEST);
}

void RHI_SetCullFace(bool enable)
{
    if (enable)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

ShaderId RHI_CreateShader(const char* vsSourec, const char* fsSource)
{
    for (uint32_t i = 1; i < MAX_SHADERS; ++i)
    {
        if (!shaderUsed[i])
        {
            shaderUsed[i] = true;
            Shader& shader = shaders[i];

            GLuint vertexShader = OpenGLCreateShader(GL_VERTEX_SHADER, vsSourec);
            GLuint fragmentShader = OpenGLCreateShader(GL_FRAGMENT_SHADER, fsSource);
            shader.id = OpenGLCreateProgram(vertexShader, fragmentShader);

            if (shader.id == 0)
            {
                shaderUsed[i] = false;
                return 0;
            }
            return i;
        }
    }
    return 0;
}

void RHI_DestroyShader(ShaderId shaderId)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    glDeleteProgram(shader.id);
    shaderUsed[shaderId] = false;
}

void RHI_BindShader(ShaderId shaderId)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    glUseProgram(shader.id);
}

void RHI_UnbindShader()
{
    glUseProgram(0);
}

void RHI_SetShaderUniformInt(ShaderId shaderId, const char* name, s32 value)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    GLint location = glGetUniformLocation(shader.id, name);
    if (location != -1)
    {
        glUniform1i(location, value);
    }
}

void RHI_SetShaderUniformFloat(ShaderId shaderId, const char* name, f32 value)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    GLint location = glGetUniformLocation(shader.id, name);
    if (location != -1)
    {
        glUniform1f(location, value);
    }
}

void RHI_SetShaderUniformVec2(ShaderId shaderId, const char* name, const glm::vec2& value)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    GLint location = glGetUniformLocation(shader.id, name);
    if (location != -1)
    {
        glUniform2f(location, value.x, value.y);
    }
}

void RHI_SetShaderUniformVec3(ShaderId shaderId, const char* name, const glm::vec3& value)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    GLint location = glGetUniformLocation(shader.id, name);
    if (location != -1)
    {
        glUniform3f(location, value.x, value.y, value.z);
    }

}

void RHI_SetShaderUniformVec4(ShaderId shaderId, const char* name, const glm::vec4& value)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    GLint location = glGetUniformLocation(shader.id, name);
    if (location != -1)
    {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

}

void RHI_SetShaderUniformMat4(ShaderId shaderId, const char* name, const glm::mat4& value)
{
    if (shaderId == 0 || shaderId >= MAX_SHADERS)
        return;

    Shader& shader = shaders[shaderId];
    GLint location = glGetUniformLocation(shader.id, name);
    if (location != -1)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
}

VertexBufferId RHI_CreateVertexBuffer(const void* data, u32 size, const BufferLayout& layout, BufferUsage usage)
{
    for (uint32_t i = 1; i < MAX_VERTEX_BUFFERS; ++i)
    {
        if (!vertexBufferUsed[i])
        {
            vertexBufferUsed[i] = true;
            VertexBuffer& vertexBuffer = vertexBuffers[i];

            glGenVertexArrays(1, &vertexBuffer.vertexArray);
            glBindVertexArray(vertexBuffer.vertexArray);

            glGenBuffers(1, &vertexBuffer.id);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.id);
            glBufferData(GL_ARRAY_BUFFER, size, data, BufferUsageToOpenGL(usage));

            vertexBuffer.size = size;
            vertexBuffer.usage = usage;

            u64 offset = 0;
            for (u32 j = 0; j < layout.count; ++j)
            {
                const BufferElement& element = layout.elements[j];
                glEnableVertexAttribArray(element.location);
                glVertexAttribPointer(element.location, AttribTypeToSize(element.type), AttribTypToOpenGL(element.type), GL_FALSE, layout.stride, (void*)offset);
                offset += AttribTypeToSize(element.type) * sizeof(float);
            }

            return i;
        }
    }
    return 0;
}

void RHI_DestroyVertexBuffer(VertexBufferId vertexBufferId)
{
    if (vertexBufferId == 0 || vertexBufferId >= MAX_VERTEX_BUFFERS)
        return;

    VertexBuffer& vertexBuffer = vertexBuffers[vertexBufferId];
    glDeleteVertexArrays(1, &vertexBuffer.vertexArray);
    glDeleteBuffers(1, &vertexBuffer.id);
    vertexBufferUsed[vertexBufferId] = false;
}

void RHI_BindVertexBuffer(VertexBufferId vertexBufferId)
{
    if (vertexBufferId == 0 || vertexBufferId >= MAX_VERTEX_BUFFERS)
        return;

    VertexBuffer& vertexBuffer = vertexBuffers[vertexBufferId];
    glBindVertexArray(vertexBuffer.vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.id);
}

void RHI_UpdateVertexBuffer(VertexBufferId vertexBuffer, const void* data, u32 size, u32 offset)
{
    if (vertexBuffer == 0 || vertexBuffer >= MAX_VERTEX_BUFFERS)
        return;

    VertexBuffer& vb = vertexBuffers[vertexBuffer];
    glBindBuffer(GL_ARRAY_BUFFER, vb.id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void RHI_UnbindVertexBuffer()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBufferId RHI_CreateIndexBuffer(const u32* indices, u32 count, BufferUsage usage)
{
    for (uint32_t i = 1; i < MAX_INDEX_BUFFERS; ++i)
    {
        if (!indexBufferUsed[i])
        {
            indexBufferUsed[i] = true;
            IndexBuffer& indexBuffer = indexBuffers[i];

            glGenBuffers(1, &indexBuffer.id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, BufferUsageToOpenGL(usage));

            indexBuffer.size = count;
            indexBuffer.usage = usage;

            return i;
        }
    }
    return 0;
}

void RHI_DestroyIndexBuffer(IndexBufferId indexBufferId)
{
    if (indexBufferId == 0 || indexBufferId >= MAX_INDEX_BUFFERS)
        return;

    IndexBuffer& indexBuffer = indexBuffers[indexBufferId];
    glDeleteBuffers(1, &indexBuffer.id);
    indexBufferUsed[indexBufferId] = false;
}

void RHI_BindIndexBuffer(IndexBufferId indexBufferId)
{
    if (indexBufferId == 0 || indexBufferId >= MAX_INDEX_BUFFERS)
        return;

    IndexBuffer& indexBuffer = indexBuffers[indexBufferId];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
}

void RHI_UnbindIndexBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

TextureId RHI_CreateTexture(const void* pixels, u32 width, u32 height, TextureFilter filter)
{
    for (uint32_t i = 1; i < MAX_TEXTURES; ++i)
    {
        if (!textureUsed[i])
        {
            textureUsed[i] = true;
            Texture& texture = textures[i];

            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToOpenGL(filter));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToOpenGL(filter));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glGenerateMipmap(GL_TEXTURE_2D);

            texture.width = width;
            texture.height = height;

            return i;
        }
    }
    return 0;
}

void RHI_DestroyTexture(TextureId textureId)
{
    if (textureId == 0 || textureId >= MAX_TEXTURES)
        return;

    Texture& texture = textures[textureId];
    glDeleteTextures(1, &texture.id);
    textureUsed[textureId] = false;
}

void RHI_BindTexture(TextureId textureId, u32 slot)
{
    if (textureId == 0 || textureId >= MAX_TEXTURES)
        return;

    Texture& texture = textures[textureId];
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

void RHI_UnbindTexture(TextureId textureId)
{
    if (textureId == 0 || textureId >= MAX_TEXTURES)
        return;

    Texture& texture = textures[textureId];
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RHI_SetTextureFilter(TextureId texture, TextureFilter min, TextureFilter mag)
{
    glBindTexture(GL_TEXTURE_2D, textures[texture].id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToOpenGL(min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToOpenGL(mag));
}

FramebufferId RHI_CreateFramebuffer(u32 width, u32 height)
{
    for (uint32_t i = 1; i < MAX_FRAMEBUFFERS; ++i)
    {
        if (!framebufferUsed[i])
        {
            framebufferUsed[i] = true;
            Framebuffer& framebuffer = framebuffers[i];

            glGenFramebuffers(1, &framebuffer.id);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

            for (uint32_t i = 1; i < MAX_TEXTURES; ++i)
            {
                if (!textureUsed[i])
                {
                    textureUsed[i] = true;
                    Texture& texture = textures[i];
                    texture.width = width;
                    texture.height = height;

                    glGenTextures(1, &texture.id);
                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id, 0);

                    framebuffer.colorAttachment = i;
                    break;
                }
            }

            for (uint32_t i = 1; i < MAX_TEXTURES; ++i)
            {
                if (!textureUsed[i])
                {
                    textureUsed[i] = true;
                    Texture& texture = textures[i];
                    texture.width = width;
                    texture.height = height;

                    glGenTextures(1, &texture.id);
                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);

                    framebuffer.depthAttachment = i;
                    break;
                }
            }

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                Platform_LogWarning("Framebuffer is not complete!\n");
                return 0;
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            framebuffer.width = width;
            framebuffer.height = height;

            return i;
        }
    }
    return 0;
}

void RHI_DestroyFramebuffer(FramebufferId framebufferId)
{
    if (framebufferId == 0 || framebufferId >= MAX_FRAMEBUFFERS)
        return;

    Framebuffer& framebuffer = framebuffers[framebufferId];
    RHI_DestroyTexture(framebuffer.colorAttachment);
    RHI_DestroyTexture(framebuffer.depthAttachment);
    glDeleteFramebuffers(1, &framebuffer.id);
    framebufferUsed[framebufferId] = false;
}

void RHI_BindFramebuffer(FramebufferId framebufferId)
{
    if (framebufferId == 0 || framebufferId >= MAX_FRAMEBUFFERS)
        return;

    Framebuffer& framebuffer = framebuffers[framebufferId];
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    glViewport(0, 0, framebuffer.width, framebuffer.height);
}

void RHI_UnbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Platform_GetWindowWidth(), Platform_GetWindowHeight());
}

TextureId RHI_GetFramebufferTexture(FramebufferId framebuffer)
{
    if (framebuffer == 0 || framebuffer >= MAX_FRAMEBUFFERS)
        return 0;

    Framebuffer& fb = framebuffers[framebuffer];
    return fb.colorAttachment;
}

TextureId RHI_GetFramebufferDepthTexture(FramebufferId framebuffer)
{
    if (framebuffer == 0 || framebuffer >= MAX_FRAMEBUFFERS)
        return 0;

    Framebuffer& fb = framebuffers[framebuffer];
    return fb.depthAttachment;
}

s32 RHI_GetFramebufferWidth(FramebufferId framebuffer)
{
    if (framebuffer == 0 || framebuffer >= MAX_FRAMEBUFFERS)
        return 0;

    Framebuffer& fb = framebuffers[framebuffer];
    return fb.width;
}

s32 RHI_GetFramebufferHeight(FramebufferId framebuffer)
{
    if (framebuffer == 0 || framebuffer >= MAX_FRAMEBUFFERS)
        return 0;

    Framebuffer& fb = framebuffers[framebuffer];
    return fb.height;
}

void RHI_Draw(u32 vertexCount, u32 offset)
{
    glDrawArrays(drawMode, offset, vertexCount);
}

void RHI_DrawIndexed(u32 vertexCount)
{
    glDrawElements(drawMode, vertexCount, GL_UNSIGNED_INT, 0);
}

void RHI_SetDrawMode(DrawMode mode)
{
    switch (mode)
    {
        case DrawMode_Triangles: drawMode = GL_TRIANGLES; break;
        case DrawMode_TriangleFan: drawMode = GL_TRIANGLE_FAN; break;
        case DrawMode_TriangleStrip: drawMode = GL_TRIANGLE_STRIP; break;
        case DrawMode_Lines: drawMode = GL_LINES; break;
        default: drawMode = GL_TRIANGLES; break;
    }
}