/**
 * @file renderer_internal.hpp
 * @brief Internal helpers shared across the renderer's translation units.
 *
 * The batch renderer is split across renderer.cpp (public API), renderer_batch.cpp
 * (CPU-side staging and sorting) and renderer_gl.cpp (OpenGL resource creation
 * and draw submission). This header declares the cross-file internals so each
 * file stays small and focused.
 */
#pragma once

#include "common/core/typedefs.hpp"
#include "common/graphics/rendering/renderer.hpp"

/// @brief Packs a shader and texture into a single sortable 64-bit key.
u64 renderer_make_key(u32 shader_id, u32 texture);

/// @brief Extracts the shader_id from a packed sort key.
u32 renderer_key_shader(u64 key);

/// @brief Extracts the texture from a packed sort key.
u32 renderer_key_texture(u64 key);

/// @brief Writes the four vertices of a rotated quad into the staging buffer.
void renderer_write_quad_vertices(Renderer* renderer, const QuadConfig* quad, u32 index);

/// @brief Sorts quads into per-shader buckets, preserving submission order.
void renderer_sort_keys(Renderer* renderer);

/// @brief Reorders the staging buffer to match the sorted quad order.
void renderer_reorder_vertices(Renderer* renderer);

/// @brief Draws the sorted quads, one draw call per (shader, texture) run.
void renderer_draw_sorted(Renderer* renderer);

/// @brief Sorts and draws all queued quads, then resets the batch cursor.
void renderer_flush(Renderer* renderer);
