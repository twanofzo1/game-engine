/**
 * @file renderer.cpp
 * @brief 2D batch renderer public API.
 *
 * Collects quads across the frame, then sorts them by (shader, texture) and
 * draws them in one call per group. CPU staging/sorting lives in
 * renderer_batch.cpp and OpenGL resource setup/draw submission lives in
 * renderer_gl.cpp.
 */
#include "common/graphics/rendering/renderer.hpp"
#include "common/graphics/rendering/renderer_internal.hpp"
#include "common/graphics/shader/shader.hpp"
#include "common/log/log.hpp"

#include <glad/glad.h>

#include <expected>

/** Default vertex shader: world-positioned, colored, textured quads. */
static const char* VERT_SRC =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec2 aUV;\n"
    "layout(location = 2) in vec4 aColor;\n"
    "uniform mat4 uViewProj;\n"
    "out vec2 vUV;\n"
    "out vec4 vColor;\n"
    "void main() {\n"
    "    vUV = aUV;\n"
    "    vColor = aColor;\n"
    "    gl_Position = uViewProj * vec4(aPos, 1.0);\n"
    "}\n";

/** Default fragment shader: modulate the texture sample by the vertex color. */
static const char* FRAG_SRC =
    "#version 330 core\n"
    "in vec2 vUV;\n"
    "in vec4 vColor;\n"
    "uniform sampler2D uTexture;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vColor * texture(uTexture, vUV);\n"
    "}\n";

/// @brief Returns the total bytes needed for the renderer's CPU-side staging buffers.
/// @return The total byte count.
static usize renderer_buffers_size()
{
    const usize vertices = sizeof(Vertex) * RENDERER_MAX_QUADS * VERTICES_PER_QUAD;
    const usize scratch  = sizeof(Vertex) * RENDERER_MAX_QUADS * VERTICES_PER_QUAD;
    const usize keys     = sizeof(u64)    * RENDERER_MAX_QUADS;
    const usize order    = sizeof(u32)    * RENDERER_MAX_QUADS;
    const usize order_tmp = sizeof(u32)   * RENDERER_MAX_QUADS;
    const usize indices  = sizeof(u32)    * RENDERER_MAX_QUADS * INDICES_PER_QUAD;
    return vertices + scratch + keys + order + order_tmp + indices;
}

/// @brief Carves the CPU-side staging buffers out of the renderer's arena.
/// @param renderer The renderer owning the arena and receiving the pointers.
/// @return Success, or OutOfMemory if any allocation failed.
static RendererError renderer_alloc_staging(Renderer* const renderer)
{
    const usize quad_bytes = sizeof(Vertex) * RENDERER_MAX_QUADS * VERTICES_PER_QUAD;
    const usize order_bytes = sizeof(u32) * RENDERER_MAX_QUADS;

    const std::expected<void*, ArenaError> vertices  = renderer->arena.alloc_aligned(quad_bytes, alignof(Vertex));
    const std::expected<void*, ArenaError> scratch   = renderer->arena.alloc_aligned(quad_bytes, alignof(Vertex));
    const std::expected<void*, ArenaError> keys      = renderer->arena.alloc_aligned(sizeof(u64) * RENDERER_MAX_QUADS, alignof(u64));
    const std::expected<void*, ArenaError> order     = renderer->arena.alloc_aligned(order_bytes, alignof(u32));
    const std::expected<void*, ArenaError> order_tmp = renderer->arena.alloc_aligned(order_bytes, alignof(u32));

    if (!vertices || !scratch || !keys || !order || !order_tmp)
    {
        log_error(LOG_SECTION_RENDERER, "renderer_alloc_staging: out of memory");
        return RendererError::OutOfMemory;
    }

    renderer->vertices  = static_cast<Vertex*>(*vertices);
    renderer->scratch   = static_cast<Vertex*>(*scratch);
    renderer->sort_keys = static_cast<u64*>(*keys);
    renderer->order     = static_cast<u32*>(*order);
    renderer->order_tmp = static_cast<u32*>(*order_tmp);
    return RendererError::Success;
}

/// @brief Compiles the built-in default shader and binds its sampler.
/// @param renderer The renderer receiving the default shader id.
/// @return Success, or ShaderCompile on failure.
static RendererError renderer_create_default_shader(Renderer* const renderer)
{
    const std::optional<ShaderID> shader =
        Shader::create({.vs_source = VERT_SRC, .fs_source = FRAG_SRC});
    if (!shader.has_value())
        return RendererError::ShaderCompile;

    renderer->default_shader = *shader;
    renderer->u_texture = Shader::uniform_location(*shader, "uTexture");

    /* Bind the color texture sampler to texture unit 0 once. */
    Shader::bind(*shader);
    glUniform1i(renderer->u_texture, 0);
    return RendererError::Success;
}

/// @brief Creates a renderer.
/// @return The renderer, or an error code on failure.
std::expected<Renderer, RendererError> Renderer::create()
{
    log_debug(LOG_SECTION_RENDERER, "create");

    const ArenaConfig arena_config = {
        .reserved = renderer_buffers_size(),
        .commit   = renderer_buffers_size(),
    };
    const std::expected<Arena, ArenaError> arena = Arena::make(arena_config);
    if (!arena)
        return std::unexpected(RendererError::OutOfMemory);

    Renderer renderer;
    renderer.arena           = *arena;
    renderer.quad_count      = 0;
    renderer.total_quads     = 0;
    renderer.default_shader  = 0;
    renderer.u_texture       = -1;

    /* Carve the staging buffers out of the arena first: on failure we still own
     * the arena and must release it before returning. */
    const RendererError staging_error = renderer_alloc_staging(&renderer);
    if (staging_error != RendererError::Success)
    {
        renderer.arena.destroy();
        return std::unexpected(staging_error);
    }

    const RendererError shader_error = renderer_create_default_shader(&renderer);
    if (shader_error != RendererError::Success)
    {
        log_debug(LOG_SECTION_RENDERER, "default shader compile failed");
        renderer.arena.destroy();
        return std::unexpected(shader_error);
    }

    renderer.create_white_texture();

    /* Alpha blending so transparent textures composite over the background. */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer.create_buffers();

    log_debug(LOG_SECTION_RENDERER, "ready (default_shader=%u)", renderer.default_shader);
    return renderer;
}

/// @brief Frees the renderer's GL resources and CPU buffers.
void Renderer::destroy()
{
    log_debug(LOG_SECTION_RENDERER, "shutdown");
    Shader::destroy_all();

    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
    glDeleteTextures(1, &this->white_texture);

    this->arena.destroy();
    this->vertices   = nullptr;
    this->scratch    = nullptr;
    this->sort_keys  = nullptr;
    this->order      = nullptr;
    this->order_tmp  = nullptr;
}

/// @brief Begins a frame with the given camera and resets the batch.
/// @param camera The camera to use this frame.
void Renderer::begin(const Camera* const camera)
{
    if (camera == nullptr)
        return;

    this->camera      = *camera;
    this->quad_count  = 0;
    this->total_quads = 0;
}

/// @brief Queues a quad; it is sorted and batched by shader/texture at end().
/// @param quad The quad to queue.
void Renderer::push_quad(const QuadConfig* const quad)
{
    if (quad == nullptr)
        return;

    /* Flush when the batch is full so any number of quads can be submitted. */
    if (this->quad_count >= RENDERER_MAX_QUADS)
        renderer_flush(this);

    const u32 shader_id = quad->shader_id != 0 ? quad->shader_id : this->default_shader;
    const u32 texture   = quad->texture   != 0 ? quad->texture   : this->white_texture;

    const u32 index = this->quad_count;
    this->sort_keys[index] = renderer_make_key(shader_id, texture);
    renderer_write_quad_vertices(this, quad, index);

    ++this->quad_count;
    ++this->total_quads;
}

/// @brief Sorts and draws all queued quads.
void Renderer::end()
{
    log_debug(LOG_SECTION_RENDERER, "end frame (%u quads)", this->quad_count);
    renderer_flush(this);
}

