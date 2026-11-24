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
    static std::optional<ShaderID> create(const ShaderConfig config);

    static void destroy(ShaderID shader);

    static void bind(ShaderID shader);

    static i32 uniform_location(ShaderID shader, const char* name);

    static void set_float(ShaderID shader, const char* name, f32 value);

    static void set_int(ShaderID shader, const char* name, i32 value);

    static void set_uint(ShaderID shader, const char* name, u32 value);

    static void set_vec2(ShaderID shader, const char* name, const f32* v);

    static void set_vec3(ShaderID shader, const char* name, const f32* v);

    static void set_vec4(ShaderID shader, const char* name, const f32* v);

    static void set_mat3(ShaderID shader, const char* name, const f32* m);

    static void set_mat4(ShaderID shader, const char* name, const f32* m);

    static void set_int_location(ShaderID shader, i32 location, i32 value);

    static void destroy_all();
};
