/**
 * @file shader.hpp
 * @brief Declares the OpenGL shader program helper.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Render {

/**
 * @brief Compile and link a vertex + fragment shader program.
 * @return Program name, or 0 on failure.
 */
u32 shader_create(const char* vs_source, const char* fs_source);

/** @brief Delete a shader program. */
void shader_destroy(u32 program);

/** @brief Bind a shader program for subsequent draw calls. */
void shader_bind(u32 program);

/** @brief Set a 4x4 float uniform (column-major). */
void shader_set_mat4(u32 program, const char* name, const f32* m);

} // namespace Render
} // namespace Engine
