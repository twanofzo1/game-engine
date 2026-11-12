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
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"
#include "core/frontend/rendering/vertex.hpp"
#include "core/frontend/camera/camera.hpp"

/** Maximum quads per batch. */
#define RENDERER_MAX_QUADS 8192

/** Parameters for a single quad drawn by the renderer. */
struct QuadConfig {
    Vec2 position;  ///< Center position in world space.
    Vec2 size;      ///< Width and height in world units.
    Color color;    ///< Color that modulates the texture sample.
    u32  texture;   ///< OpenGL texture name (0 = solid white).
    f32  rotation;  ///< Rotation in radians around the center.
    f32  u0;        ///< Left edge of the UV rectangle.
    f32  v0;        ///< Top edge of the UV rectangle.
    f32  u1;        ///< Right edge of the UV rectangle.
    f32  v1;        ///< Bottom edge of the UV rectangle.
};

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

    /** @brief Initialise the renderer, creating its GL resources. */
    static void init(Renderer* renderer);

    /** @brief Free the renderer's GL resources and CPU buffers. */
    static void shutdown(Renderer* renderer);

    /** @brief Begin a frame with the given camera and reset the batch. */
    static void begin(Renderer* renderer, const Camera* camera);

    /** @brief Queue a quad to be drawn; flushes when the batch fills or texture changes. */
    static void push_quad(Renderer* renderer, const QuadConfig* quad);

    /** @brief Upload and draw all queued quads. */
    static void end(Renderer* renderer);
};


