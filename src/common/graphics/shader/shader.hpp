/**
 * @file shader.hpp
 * @brief Declares the OpenGL shader program helper.
 */
#pragma once

#include "common/core/typedefs.hpp"

#include <optional>

using ShaderID = u32; ///< OpenGL shader program name (GLuint).

/** @brief Configuration used to compile and link a shader program. */
struct ShaderConfig {
    const char* vs_source = nullptr; ///< Vertex shader source code.
    const char* fs_source = nullptr; ///< Fragment shader source code.
};

/** @brief An OpenGL shader program (vertex + fragment stages linked). */
struct Shader {

    /** @brief Compile and link a program, or nullopt on failure. */
    static std::optional<ShaderID> make(ShaderConfig config);

    /** @brief Delete the program and reset its name to 0. */
    static void destroy(ShaderID shader);

    /** @brief Bind the program for subsequent draw calls. */
    static void bind(ShaderID shader);

    /** @brief Look up a uniform location by name (-1 if absent). */
    static i32 uniform_location(ShaderID shader, const char* name);

    /** @brief Set a scalar float uniform by name. */
    static void set_float(ShaderID shader, const char* name, f32 value);

    /** @brief Set a scalar int uniform by name. */
    static void set_int(ShaderID shader, const char* name, i32 value);

    /** @brief Set a scalar unsigned int uniform by name. */
    static void set_uint(ShaderID shader, const char* name, u32 value);

    /** @brief Set a 2-component float uniform by name. */
    static void set_vec2(ShaderID shader, const char* name, const f32* v);

    /** @brief Set a 3-component float uniform by name. */
    static void set_vec3(ShaderID shader, const char* name, const f32* v);

    /** @brief Set a 4-component float uniform by name. */
    static void set_vec4(ShaderID shader, const char* name, const f32* v);

    /** @brief Set a 3x3 float uniform (column-major). */
    static void set_mat3(ShaderID shader, const char* name, const f32* m);

    /** @brief Set a 4x4 float uniform (column-major). */
    static void set_mat4(ShaderID shader, const char* name, const f32* m);

    /** @brief Set an int uniform by its pre-cached location. */
    static void set_int_location(ShaderID shader, i32 location, i32 value);

    /** @brief Destroy all shader programs created by the renderer. */
    static void destroy_all(); 
};
