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

/// @brief Compiles one shader stage.
/// @param type The GL shader stage type (e.g. GL_VERTEX_SHADER).
/// @param source The shader source code.
/// @return The compiled stage name, or 0 on failure.
static u32 compile_stage(const u32 type, const char* const source)
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

/// @brief Compiles and links a program from vertex and fragment sources.
/// @param config The vertex and fragment shader sources.
/// @return The linked program name, or nullopt on failure.
std::optional<Shader> Shader::create(const ShaderConfig config)
{
    log_debug(LOG_SECTION_SHADER, "create");
    const u32 vs = compile_stage(GL_VERTEX_SHADER, config.vs_source);
    const u32 fs = compile_stage(GL_FRAGMENT_SHADER, config.fs_source);
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
    return Shader{.id = program};
}

/// @brief Deletes the program and resets its name to 0.
/// @param shader The program name.
void Shader::destroy(const ShaderID shader)
{
    if (shader != 0)
        glDeleteProgram(shader);
}

/// @brief Binds the program for subsequent draw calls.
/// @param shader The program name.
void Shader::bind(const ShaderID shader)
{
    glUseProgram(shader);
}

/// @brief Looks up a uniform location by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @return The uniform location, or -1 when absent.
i32 Shader::uniform_location(const char* const name)
{
    return glGetUniformLocation(this->id, name);
}

/// @brief Sets a scalar float uniform by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @param value The value to set.
void Shader::set_float(const char* const name, const f32 value)
{
    glUniform1f(glGetUniformLocation(this->id, name), value);
}

/// @brief Sets a scalar int uniform by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @param value The value to set.
void Shader::set_int(const char* const name, const i32 value)
{
    glUniform1i(glGetUniformLocation(this->id, name), value);
}

/// @brief Sets a scalar unsigned int uniform by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @param value The value to set.
void Shader::set_uint(const char* const name, const u32 value)
{
    glUniform1ui(glGetUniformLocation(this->id, name), value);
}

/// @brief Sets a 2-component float uniform by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @param v The two float values.
void Shader::set_vec2(const char* const name, const f32* const v)
{
    glUniform2fv(glGetUniformLocation(this->id, name), 1, v);
}

/// @brief Sets a 3-component float uniform by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @param v The three float values.
void Shader::set_vec3(const char* const name, const f32* const v)
{
    glUniform3fv(glGetUniformLocation(this->id, name), 1, v);
}

/// @brief Sets a 4-component float uniform by name.
/// @param shader The program name.
/// @param name The uniform name.
/// @param v The four float values.
void Shader::set_vec4(const char* const name, const f32* const v)
{
    glUniform4fv(glGetUniformLocation(this->id, name), 1, v);
}

/// @brief Sets a 3x3 float uniform (column-major).
/// @param shader The program name.
/// @param name The uniform name.
/// @param m The nine float values.
void Shader::set_mat3(const char* const name, const f32* const m)
{
    glUniformMatrix3fv(glGetUniformLocation(this->id, name), 1, GL_FALSE, m);
}

/// @brief Sets a 4x4 float uniform (column-major).
/// @param shader The program name.
/// @param name The uniform name.
/// @param m The sixteen float values.
void Shader::set_mat4(const char* const name, const f32* const m)
{
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, GL_FALSE, m);
}

/// @brief Sets an int uniform by its pre-cached location.
/// @param shader The program name.
/// @param location The uniform location.
/// @param value The value to set.
void Shader::set_int_location(const i32 location, const i32 value)
{
    glUniform1i(location, value);
}

