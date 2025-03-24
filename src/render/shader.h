#pragma once
#include "core/types.h"
#include "core/math.h"

struct Shader;

// Creates a new shader.
Shader* ShaderCreate(const char* vertex_shader, const char* fragment_shader);

// Loads a shader from a file.
Shader* ShaderLoad(const char* vertex_shader_path, const char* fragment_shader_path);

// Destroyes the shader.
void ShaderDestroy(Shader* shader);

// Binds the shader.
void ShaderBind(Shader* shader);

// Unbinds the shader.
void ShaderUnbind();

// Sets int uniform.
void ShaderSetInt(Shader* shader, const char* name, s32 value);

// Sets float uniform.
void ShaderSetFloat(Shader* shader, const char* name, f32 value);

// Sets vec2 uniform.
void ShaderSetVec2(Shader* shader, const char* name, Vec2 value);

// Sets vec3 uniform.
void ShaderSetVec3(Shader* shader, const char* name, Vec3 value);

// Sets vec4 uniform.
void ShaderSetVec4(Shader* shader, const char* name, Vec4 value);

// Sets mat4 uniform.
void ShaderSetMat4(Shader* shader, const char* name, Mat4 value);