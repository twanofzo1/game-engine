/**
 * @file shader.hpp
 * @brief Declares the OpenGL shader program helper.
 */
#pragma once

#include "common/core/typedefs.hpp"

#include <optional>

using ShaderID = u32; ///< OpenGL shader program name (GLuint).

/// @brief Configuration used to compile and link a shader program.
struct ShaderConfig {
    const char* vs_source = nullptr; ///< Vertex shader source code.
    const char* fs_source = nullptr; ///< Fragment shader source code.
};

/// @brief An OpenGL shader program (vertex + fragment stages linked).
struct Shader {
    u32 id = 0; ///< OpenGL shader program name (GLuint).

    static std::optional<Shader> create(const ShaderConfig config);

    void destroy(ShaderID shader);

    void bind(ShaderID shader);

    i32 uniform_location(const char* name);

    void set_float(const char* name, f32 value);

    void set_int(const char* name, i32 value);

    void set_uint(const char* name, u32 value);

    void set_vec2(const char* name, const f32* v);

    void set_vec3(const char* name, const f32* v);

    void set_vec4(const char* name, const f32* v);

    void set_mat3(const char* name, const f32* m);

    void set_mat4(const char* name, const f32* m);

    void set_int_location(i32 location, i32 value);

    void destroy();
};
