/**
 * @file renderer.cpp
 * @brief Multi-shader 2D batch renderer public API.
 *
 * Collects quads across the frame, sorts them by (shader, texture), and draws
 * them in a single call per group. The sorting/batching lives in
 * renderer_batch.cpp and the GL resource setup in renderer_gl.cpp.
 */
#include "core/frontend/rendering/renderer.hpp"
#include "core/frontend/rendering/renderer_internal.hpp"
#include "core/frontend/rendering/shader.hpp"

#include <glad/glad.h>

#include <expected>

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

static const char* FRAG_SRC =
    "#version 330 core\n"
    "in vec2 vUV;\n"
    "in vec4 vColor;\n"
    "uniform sampler2D uTexture;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vColor * texture(uTexture, vUV);\n"
    "}\n";

/** @brief Total bytes needed for the renderer's CPU-side staging buffers. */
static usize renderer_buffers_size()
{
    return sizeof(Vertex) * RENDERER_MAX_QUADS * 4   /* vertices */
         + sizeof(Vertex) * RENDERER_MAX_QUADS * 4   /* scratch  */
         + sizeof(u64)    * RENDERER_MAX_QUADS       /* sort_keys */
         + sizeof(u32)    * RENDERER_MAX_QUADS       /* order     */
         + sizeof(u32)    * RENDERER_MAX_QUADS * 6;  /* indices   */
}

/** @brief Create a renderer, or return an error code on failure. */
std::expected<Renderer, RendererError> Renderer::make()
{
    const ArenaConfig arena_config = {
        .reserved = renderer_buffers_size(),
        .commit   = renderer_buffers_size(),
    };
    const std::expected<Arena, ArenaError> arena = Arena::make(arena_config);
    if (!arena)
        return std::unexpected(RendererError::OutOfMemory);

    Renderer renderer;
    renderer.arena       = *arena;
    renderer.quad_count  = 0;
    renderer.total_quads = 0;
    renderer.program_count = 0;
    renderer.camera = Camera::make({
        .position = Vec2::make({.x = 0.0f, .y = 0.0f}),
        .size = Vec2::make({.x = 1280.0f, .y = 720.0f})
    });

    /* Carve the four staging buffers out of the arena. */
    const std::expected<void*, ArenaError> vertices = Arena::alloc_aligned(&renderer.arena, sizeof(Vertex) * RENDERER_MAX_QUADS * 4, alignof(Vertex));
    const std::expected<void*, ArenaError> scratch  = Arena::alloc_aligned(&renderer.arena, sizeof(Vertex) * RENDERER_MAX_QUADS * 4, alignof(Vertex));
    const std::expected<void*, ArenaError> keys     = Arena::alloc_aligned(&renderer.arena, sizeof(u64) * RENDERER_MAX_QUADS, alignof(u64));
    const std::expected<void*, ArenaError> order    = Arena::alloc_aligned(&renderer.arena, sizeof(u32) * RENDERER_MAX_QUADS, alignof(u32));

    if (!vertices || !scratch || !keys || !order)
    {
        Arena::destroy(&renderer.arena);
        return std::unexpected(RendererError::OutOfMemory);
    }

    renderer.vertices  = (Vertex*)*vertices;
    renderer.scratch   = (Vertex*)*scratch;
    renderer.sort_keys = (u64*)*keys;
    renderer.order     = (u32*)*order;

    /* Shader 0 is the default solid-color/textured quad shader. */
    if (!register_shader(&renderer, VERT_SRC, FRAG_SRC))
    {
        Arena::destroy(&renderer.arena);
        return std::unexpected(RendererError::ShaderCompile);
    }

    renderer_create_white_texture(&renderer);

    /* Alpha blending so transparent textures composite over the background. */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer_create_buffers(&renderer);

    return renderer;
}

/** @brief Free the renderer's GL resources and CPU buffers. */
void Renderer::shutdown(Renderer* renderer)
{
    for (u32 i = 0; i < renderer->program_count; ++i)
        shader_destroy(renderer->programs[i]);
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteBuffers(1, &renderer->ebo);
    glDeleteTextures(1, &renderer->white_texture);
    Arena::destroy(&renderer->arena);
    renderer->vertices = NULL;
    renderer->scratch = NULL;
    renderer->sort_keys = NULL;
    renderer->order = NULL;
}

/** @brief Register a shader program, returning its shader_id or an error code. */
std::expected<u32, RendererError> Renderer::register_shader(Renderer* renderer,
                                                            const char* vs_source,
                                                            const char* fs_source)
{
    if (renderer->program_count >= RENDERER_MAX_SHADERS)
        return std::unexpected(RendererError::ShaderTableFull);

    const u32 program = shader_create(vs_source, fs_source);
    if (!program)
        return std::unexpected(RendererError::ShaderCompile);

    const u32 id = renderer->program_count++;
    renderer->programs[id] = program;
    renderer->u_view_proj[id] = glGetUniformLocation(program, "uViewProj");
    renderer->u_texture[id] = glGetUniformLocation(program, "uTexture");

    /* Bind the color texture sampler to texture unit 0 once. */
    shader_bind(program);
    glUniform1i(renderer->u_texture[id], 0);

    return id;
}

/** @brief Begin a frame with the given camera and reset the batch. */
void Renderer::begin(Renderer* renderer, const Camera* camera)
{
    renderer->camera = *camera;
    renderer->quad_count = 0;
    renderer->total_quads = 0;
}

/** @brief Queue a quad; it is sorted and batched by shader/texture at end(). */
void Renderer::push_quad(Renderer* renderer, const QuadConfig* quad)
{
    if (!quad || renderer->quad_count >= RENDERER_MAX_QUADS)
        return;

    const u32 shader_id = quad->shader_id < renderer->program_count ? quad->shader_id : 0;
    const u32 texture = quad->texture ? quad->texture : renderer->white_texture;

    const u32 q = renderer->quad_count;
    renderer->sort_keys[q] = renderer_make_key(shader_id, texture);
    renderer_write_quad_vertices(renderer, quad, q);

    ++renderer->quad_count;
    ++renderer->total_quads;
}

/** @brief Sort queued quads by shader then texture, drawing each run as one call. */
void Renderer::end(Renderer* renderer)
{
    if (renderer->quad_count == 0)
        return;

    renderer_sort_keys(renderer);
    renderer_reorder_vertices(renderer);
    renderer_draw_sorted(renderer);

    renderer->quad_count = 0;
}

