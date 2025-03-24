#include "shader.h"
#include "platform/platform.h"

#include <stdio.h>
#include <string.h>
#include <glad/glad.h>

#define MAX_SHADERS 32

struct Shader
{
    u32 id;
};

static Shader shaders[MAX_SHADERS];
static s32 shader_count = 0;

static char* ReadFile(const char* path) {
    FILE* fs;
    fopen_s(&fs, path, "rb");

    if (!fs) return nullptr;

    fseek(fs, 0, SEEK_END);
    long size = ftell(fs);
    rewind(fs);

    char* buffer = (char*)PlatformAllocateMemory(size + 1);
    fread(buffer, 1, size, fs);
    buffer[size] = 0;

    fclose(fs);
    return buffer;
}

static GLuint CompileShader(const char* source, GLenum type)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        printf("Shader compile error:\n%s\n", log);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static GLuint LinkProgram(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        printf("Shader link error:\n%s\n", log);
        glDeleteProgram(program);
        return 0;
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

static Shader* GetNextAvailableShader()
{
    for (s32 i = 0; i < MAX_SHADERS; i++)
    {
        if (shaders[i].id == 0)
        {
            return &shaders[i];
        }
    }
    return nullptr;
}

Shader* ShaderCreate(const char* vertex_shader, const char* fragment_shader)
{
    GLuint vs = CompileShader(vertex_shader, GL_VERTEX_SHADER);
    GLuint fs = CompileShader(fragment_shader, GL_FRAGMENT_SHADER);
    if (!vs || !fs) return nullptr;

    GLuint program = LinkProgram(vs, fs);
    if (!program) return nullptr;

    Shader* shader = GetNextAvailableShader();
    if (!shader) return nullptr;

    shader->id = program;

    return shader;
}

Shader* ShaderLoad(const char* vertex_shader_path, const char* fragment_shader_path)
{
    char* vertex_shader = ReadFile(vertex_shader_path);
    char* fragment_shader = ReadFile(fragment_shader_path);

    Shader* shader = ShaderCreate(vertex_shader, fragment_shader);

    PlatformFreeMemory(vertex_shader);
    PlatformFreeMemory(fragment_shader);

    return shader;
}

void ShaderDestroy(Shader* shader)
{
    glDeleteProgram(shader->id);
    shader->id = 0;
}

void ShaderBind(Shader* shader)
{
    glUseProgram(shader->id);
}

void ShaderUnbind()
{
    glUseProgram(0);
}

void ShaderSetInt(Shader* shader, const char* name, s32 value)
{
    glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void ShaderSetFloat(Shader* shader, const char* name, f32 value)
{
    glUniform1f(glGetUniformLocation(shader->id, name), value);
}

void ShaderSetVec2(Shader* shader, const char* name, Vec2 value)
{
    glUniform2fv(glGetUniformLocation(shader->id, name), 1, &value.x);
}

void ShaderSetVec3(Shader* shader, const char* name, Vec3 value)
{
    glUniform3fv(glGetUniformLocation(shader->id, name), 1, &value.x);
}

void ShaderSetVec4(Shader* shader, const char* name, Vec4 value)
{
    glUniform4fv(glGetUniformLocation(shader->id, name), 1, &value.x);
}

void ShaderSetMat4(Shader* shader, const char* name, Mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, &value.m[0][0]);
}
