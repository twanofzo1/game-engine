/**
 * @file renderer.hpp
 * @brief Declares the 2D batch renderer.
 *
 * Quads are staged into a CPU vertex buffer across the whole frame, then sorted
 * by (shader_id, texture) at end() so that all quads sharing a shader and a
 * texture are drawn in a single draw call. A shader program is selected per
 * quad via shader_id; 0 selects the built-in default shader.
 */
#pragma once

#include "common/core/typedefs.hpp"
#include "common/core/types/vec2.hpp"
#include "common/core/types/color.hpp"
#include "common/core/memory/arena.hpp"
#include "common/graphics/rendering/vertex.hpp"
#include "common/graphics/shader/shader.hpp"
#include "common/graphics/camera/camera.hpp"

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
    Vec2 position;                              ///< Center position in world space.
    Vec2 size;                                  ///< Width and height in world units.
    Color color;                                ///< Color that modulates the texture sample.
    u32  texture = 0;                           ///< OpenGL texture name (0 = solid white).
    u32  shader_id = 0;                         ///< Shader program name to draw with (0 = default).
    f32  rotation = 0.0f;                       ///< Rotation in radians around the center.
    f32  u0 = 0.0f;                             ///< Left edge of the UV rectangle.
    f32  v0 = 0.0f;                             ///< Top edge of the UV rectangle.
    f32  u1 = 1.0f;                             ///< Right edge of the UV rectangle.
    f32  v1 = 1.0f;                             ///< Bottom edge of the UV rectangle.
};

/// @brief State for the 2D batch renderer.
struct Renderer {
    u32     vao;            ///< Vertex array object.
    u32     vbo;            ///< Dynamic vertex buffer.
    u32     ebo;            ///< Static index buffer.
    u32     default_shader; ///< Built-in white/textured shader (used when shader_id == 0).
    i32     u_texture;      ///< uTexture uniform location for the default shader.
    u32     white_texture;  ///< 1x1 white texture for solid-color quads.
    Arena   arena;          ///< Memory arena backing the CPU-side staging buffers.
    Vertex* vertices;       ///< CPU-side staging buffer (MAX_QUADS * 4).
    Vertex* scratch;        ///< Scratch buffer for reordering vertices (MAX_QUADS * 4).
    u64*    sort_keys;      ///< Per-quad sort key (shader_id in high bits, texture in low).
    u32*    order;          ///< Permutation of quad indices after sorting.
    u32*    order_tmp;      ///< Second permutation buffer used during the sort.
    u32     quad_count;     ///< Quads queued in the current batch.
    u32     total_quads;    ///< Total quads submitted this frame.
    Camera  camera;         ///< Camera used for the current frame.

    static std::expected<Renderer, RendererError> create();

    void destroy();

    void begin(const Camera* camera);

    void push_quad(const QuadConfig* quad);

    void end();

    /** @brief Draws a contiguous run of quads with one shader and texture. */
    void draw_run(u32 shader_id, u32 texture, u32 count);

    /** @brief Creates the 1x1 white texture used for solid-color quads. */
    void create_white_texture();

    /** @brief Creates the VAO and the dynamic/index buffers. */
    void create_buffers();
};


