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
    Vec2 position;  ///< Center position in world space.
    Vec2 size;      ///< Width and height in world units.
    Color color;    ///< Color that modulates the texture sample.
    u32  texture;   ///< OpenGL texture name (0 = solid white).
    u32  shader_id; ///< Index into the renderer's shader program table (0 = default).
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
    u32     programs[RENDERER_MAX_SHADERS];  ///< Shader programs indexed by shader_id.
    i32     u_texture[RENDERER_MAX_SHADERS];   ///< uTexture uniform per program.
    u32     program_count; ///< Number of registered shader programs.
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

    /** @brief Register a shader program, returning its shader_id or an error. */
    static std::expected<u32, RendererError> register_shader(Renderer* renderer,
                                                             const char* vs_source,
                                                             const char* fs_source);

    /** @brief Queue a quad to be drawn; sorted and batched by shader/texture at end(). */
    static void push_quad(Renderer* renderer, const QuadConfig* quad);

    /** @brief Upload and draw all queued quads, sorted by shader then texture. */
    static void end(Renderer* renderer);
};


