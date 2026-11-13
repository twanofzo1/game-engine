/**
 * @file renderer_internal.hpp
 * @brief Internal helpers shared across the renderer's translation units.
 *
 * The batch renderer is split across renderer.cpp (public API), renderer_batch.cpp
 * (sort/batch logic) and renderer_gl.cpp (GL resource setup). This header declares
 * the cross-file internals so each file stays small and focused.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/frontend/rendering/renderer.hpp"


/** @brief Pack a shader and texture into a single sortable 64-bit key. */
u64 renderer_make_key(u32 shader_id, u32 texture);

/** @brief Extract the shader_id from a packed sort key. */
u32 renderer_key_shader(u64 key);

/** @brief Extract the texture from a packed sort key. */
u32 renderer_key_texture(u64 key);

/** @brief Draw a contiguous run of quads with one shader and texture. */
void renderer_draw_run(Renderer* renderer, u32 shader_id, u32 texture, u32 count);

/** @brief Create the 1x1 white texture used for solid-color quads. */
void renderer_create_white_texture(Renderer* renderer);

/** @brief Create the VAO and the dynamic/index buffers. */
void renderer_create_buffers(Renderer* renderer);

/** @brief Write the four vertices of a rotated quad into the staging buffer. */
void renderer_write_quad_vertices(Renderer* renderer, const QuadConfig* quad, u32 index);

/** @brief Sort quads into per-shader buckets, preserving submission order. */
void renderer_sort_keys(Renderer* renderer);

/** @brief Reorder the staging buffer to match the sorted quad order. */
void renderer_reorder_vertices(Renderer* renderer);

/** @brief Draw the sorted quads, one draw call per (shader, texture) run. */
void renderer_draw_sorted(Renderer* renderer);
