/**
 * @file shader.cpp
 * @brief Minimal OpenGL shader program helper implementation.
 */
#include "graphics/rendering/shader.hpp"

#include <glad/glad.h>

#include <stdio.h>

namespace Engine {
namespace Render {

/**
 * @brief Compile one shader stage.
 * @return Stage name, or 0 on failure.
 */
static u32 compile_stage(u32 type, const char* source)
{
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetShaderInfoLog(shader, (GLsizei)sizeof(log), NULL, log);
        fprintf(stderr, "shader compile error (%u): %s\n", type, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

u32 shader_create(const char* vs_source, const char* fs_source)
{
    u32 vs = compile_stage(GL_VERTEX_SHADER, vs_source);
    u32 fs = compile_stage(GL_FRAGMENT_SHADER, fs_source);
    if (!vs || !fs)
    {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }

    u32 program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetProgramInfoLog(program, (GLsizei)sizeof(log), NULL, log);
        fprintf(stderr, "program link error: %s\n", log);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void shader_destroy(u32 program)
{
    glDeleteProgram(program);
}

void shader_bind(u32 program)
{
    glUseProgram(program);
}

void shader_set_mat4(u32 program, const char* name, const f32* m)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, m);
}

} // namespace Render
} // namespace Engine
