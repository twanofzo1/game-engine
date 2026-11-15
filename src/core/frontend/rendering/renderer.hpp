/**
 * @file renderer.hpp
 * @brief Declares the multi-shader 2D batch renderer.
 *
 * Quads are staged in a CPU buffer across the whole frame, then sorted by
 * (shader_id, texture) at end() so that all quads sharing a shader and a
 * texture are drawn in a single draw call. Shader programs are registered into
 * an indexed table and selected per-quad via shader_id.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"
#include "core/memory/arena.hpp"
#include "core/frontend/rendering/vertex.hpp"
#include "core/frontend/rendering/shader.hpp"
#include "core/frontend/camera/camera.hpp"

#include <expected>

/** Maximum quads per batch. */
#define RENDERER_MAX_QUADS 8192

/** Maximum number of distinct shader programs the renderer can hold. */
#define RENDERER_MAX_SHADERS 256

/** Error codes returned by fallible renderer operations. */
enum class RendererError : u8 {
    Success         = 0, ///< No error.
    OutOfMemory     = 1, ///< A CPU-side buffer allocation failed.
    ShaderTableFull = 2, ///< No free slot remains in the shader program table.
    ShaderCompile   = 3, ///< A shader failed to compile or link.
};

/** Parameters for a single quad drawn by the renderer. */
struct QuadConfig {
    Vec2 position = Vec2::make({.x = 0.0f, .y = 0.0f});  ///< Center position in world space.
    Vec2 size = Vec2::make({.x = 1.0f, .y = 1.0f});      ///< Width and height in world units.
    Color color = Color::make({.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f});    ///< Color that modulates the texture sample.
    u32  texture = 0;   ///< OpenGL texture name (0 = solid white).
    u32  shader_id = 0; ///< Shader program name to draw with (0 = default).
    f32  rotation = 0.0f;  ///< Rotation in radians around the center.
    f32  u0 = 0.0f;        ///< Left edge of the UV rectangle.
    f32  v0 = 0.0f;        ///< Top edge of the UV rectangle.
    f32  u1 = 1.0f;        ///< Right edge of the UV rectangle.
    f32  v1 = 1.0f;        ///< Bottom edge of the UV rectangle.
};


/** State for the 2D batch renderer. */
struct Renderer {
    u32     vao;         ///< Vertex array object.
    u32     vbo;         ///< Dynamic vertex buffer.
    u32     ebo;         ///< Static index buffer.
    u32     default_shader; ///< Built-in white/textured shader (used when shader_id == 0).
    i32     u_texture;   ///< uTexture uniform location for the default shader.
    u32     white_texture;   ///< 1x1 white texture for solid-color quads.
    Arena   arena;           ///< Memory arena backing the CPU-side staging buffers.
    Vertex* vertices;        ///< CPU-side staging buffer (MAX_QUADS * 4).
    Vertex* scratch;         ///< Scratch buffer for reordering vertices (MAX_QUADS * 4).
    u64*    sort_keys;       ///< Per-quad sort key (shader_id in high bits, texture in low).
    u32*    order;           ///< Scratch permutation of quad indices.
    u32     quad_count;  ///< Quads queued in the current batch.
    u32     total_quads; ///< Total quads submitted this frame.
    Camera  camera;      ///< Camera used for the current frame.

    /** @brief Create a renderer, or an error code on failure. */
    static std::expected<Renderer, RendererError> make();

    /** @brief Free the renderer's GL resources and CPU buffers. */
    static void shutdown(Renderer* renderer);

    /** @brief Begin a frame with the given camera and reset the batch. */
    static void begin(Renderer* renderer, const Camera* camera);

    /** @brief Queue a quad to be drawn; sorted and batched by shader/texture at end(). */
    static void push_quad(Renderer* renderer, const QuadConfig* quad);

    /** @brief Upload and draw all queued quads, sorted by shader then texture. */
    static void end(Renderer* renderer);
};


