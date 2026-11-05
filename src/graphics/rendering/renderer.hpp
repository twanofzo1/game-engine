/**
 * @file renderer.hpp
 * @brief Declares the 2D batch renderer.
 *
 * The renderer collects quads in a CPU-side staging buffer and draws them in
 * fixed-size batches. Each batch is one draw call; when a batch fills up (or
 * the texture changes) it is flushed and a new one starts, so any number of
 * quads can be drawn. It handles solid-color and textured quads.
 */
#pragma once

#include "core/typedefs.hpp"
#include "graphics/rendering/vertex.hpp"
#include "graphics/rendering/camera.hpp"
#include "core/ecs/components.hpp"

/** Maximum quads per batch. */
#define RENDERER_MAX_QUADS 8192

namespace Engine {
namespace Render {

/** State for the 2D batch renderer. */
struct Renderer {
    u32     vao;         ///< Vertex array object.
    u32     vbo;         ///< Dynamic vertex buffer.
    u32     ebo;         ///< Static index buffer.
    u32     program;     ///< Shader program.
    i32     u_view_proj;     ///< Location of the view-projection uniform.
    i32     u_texture;       ///< Location of the texture sampler uniform.
    u32     white_texture;   ///< 1x1 white texture for solid-color quads.
    u32     current_texture; ///< Texture bound for the current batch.
    Vertex* vertices;        ///< CPU-side staging buffer (MAX_QUADS * 4).
    u32     quad_count;  ///< Quads queued in the current batch.
    u32     total_quads; ///< Total quads submitted this frame.
    Camera  camera;      ///< Camera used for the current frame.
};

void renderer_init(Renderer* renderer);
void renderer_shutdown(Renderer* renderer);

/** @brief Begin a frame with the given camera and reset the batch. */
void renderer_begin(Renderer* renderer, const Camera* camera);

/** @brief Queue one colored quad from a transform + sprite. */
void renderer_push_quad(Renderer* renderer, const Ecs::Transform* transform, const Ecs::Sprite* sprite);

/** @brief Upload and draw all queued quads. */
void renderer_end(Renderer* renderer);

} // namespace Render
} // namespace Engine
