/**
 * @file shader.cpp
 * @brief OpenGL shader program helper implementation.
 */
#include "common/graphics/shader/shader.hpp"
#include "common/log/log.hpp"

#include <glad/glad.h>

#include <stdio.h>

#include <vector>

/** @brief Registry of every live program, used by destroy_all. */
static std::vector<ShaderID> g_programs;

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
        log_error(LOG_SECTION_SHADER, "compile error (%u): %s", type, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

/** @brief Compile and link a program, or nullopt on failure. */
std::optional<ShaderID> Shader::make(ShaderConfig config)
{
    log_debug(LOG_SECTION_SHADER, "make");
    u32 vs = compile_stage(GL_VERTEX_SHADER, config.vs_source);
    u32 fs = compile_stage(GL_FRAGMENT_SHADER, config.fs_source);
    if (!vs || !fs)
    {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return std::nullopt;
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
        log_error(LOG_SECTION_SHADER, "link error: %s", log);
        glDeleteProgram(program);
        program = 0;
    } else {
        g_programs.push_back(program);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    log_debug(LOG_SECTION_SHADER, "program %u ready", program);
    return program;
}

/** @brief Delete the program and reset its name to 0. */
void Shader::destroy(ShaderID shader)
{
    if (shader != 0)
    {
        glDeleteProgram(shader);
        shader = 0;
    }
}

/** @brief Bind the program for subsequent draw calls. */
void Shader::bind(ShaderID shader)
{
    glUseProgram(shader);
}

/** @brief Look up a uniform location by name (-1 if absent). */
i32 Shader::uniform_location(ShaderID shader, const char* name)
{
    return glGetUniformLocation(shader, name);
}

/** @brief Set a scalar float uniform by name. */
void Shader::set_float(ShaderID shader, const char* name, f32 value)
{
    glUniform1f(glGetUniformLocation(shader, name), value);
}

/** @brief Set a scalar int uniform by name. */
void Shader::set_int(ShaderID shader, const char* name, i32 value)
{
    glUniform1i(glGetUniformLocation(shader, name), value);
}

/** @brief Set a scalar unsigned int uniform by name. */
void Shader::set_uint(ShaderID shader, const char* name, u32 value)
{
    glUniform1ui(glGetUniformLocation(shader, name), value);
}

/** @brief Set a 2-component float uniform by name. */
void Shader::set_vec2(ShaderID shader, const char* name, const f32* v)
{
    glUniform2fv(glGetUniformLocation(shader, name), 1, v);
}

/** @brief Set a 3-component float uniform by name. */
void Shader::set_vec3(ShaderID shader, const char* name, const f32* v)
{
    glUniform3fv(glGetUniformLocation(shader, name), 1, v);
}

/** @brief Set a 4-component float uniform by name. */
void Shader::set_vec4(ShaderID shader, const char* name, const f32* v)
{
    glUniform4fv(glGetUniformLocation(shader, name), 1, v);
}

/** @brief Set a 3x3 float uniform (column-major). */
void Shader::set_mat3(ShaderID shader, const char* name, const f32* m)
{
    glUniformMatrix3fv(glGetUniformLocation(shader, name), 1, GL_FALSE, m);
}

/** @brief Set a 4x4 float uniform (column-major). */
void Shader::set_mat4(ShaderID shader, const char* name, const f32* m)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, m);
}

/** @brief Set an int uniform by its pre-cached location. */
void Shader::set_int_location(ShaderID shader, i32 location, i32 value)
{
    glUniform1i(location, value);
}

/** @brief Destroy every program created by Shader::make. */
void Shader::destroy_all()
{
    for (u32 i = 0; i < g_programs.size(); i++) {
        glDeleteProgram(g_programs[i]);
    }
    g_programs.clear();
}