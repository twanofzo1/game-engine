/**
 * @file renderer_gpu.hpp
 * @brief Shared GPU resources for the batched quad renderer.
 */
#pragma once

#include "common/core/typedefs.hpp"

/** Shared VAO/VBO/EBO and white texture names. */
extern u32 g_vao;
extern u32 g_vbo;
extern u32 g_ebo;
extern u32 g_white_texture;

/// @brief Lazily creates the shared VAO/VBO/EBO and white texture.
void renderer_gpu_ensure_resources();
