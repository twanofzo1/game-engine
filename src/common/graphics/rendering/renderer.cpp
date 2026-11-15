/**
 * @file renderer.cpp
 * @brief Multi-shader 2D batch renderer public API.
 *
 * Collects quads across the frame, sorts them by (shader, texture), and draws
 * them in a single call per group. The sorting/batching lives in
 * renderer_batch.cpp and the GL resource setup in renderer_gl.cpp.
 */
#include "common/graphics/rendering/renderer.hpp"
#include "common/graphics/rendering/renderer_internal.hpp"
#include "common/graphics/shader/shader.hpp"

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
    renderer.default_shader = 0;
    renderer.u_texture = -1;


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

    /* The built-in white/textured quad shader used whenever shader_id == 0. */
    const std::optional<ShaderID> default_shader =
        Shader::make({.vs_source = VERT_SRC, .fs_source = FRAG_SRC});
    if (!default_shader.has_value())
    {
        Arena::destroy(&renderer.arena);
        return std::unexpected(RendererError::ShaderCompile);
    }
    renderer.default_shader = *default_shader;
    renderer.u_texture = Shader::uniform_location(*default_shader, "uTexture");

    /* Bind the color texture sampler to texture unit 0 once. */
    Shader::bind(*default_shader);
    glUniform1i(renderer.u_texture, 0);

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
    Shader::destroy_all();

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
    if (!quad)
        return;

    /* Flush when the batch is full so any number of quads can be submitted. */
    if (renderer->quad_count >= RENDERER_MAX_QUADS)
        renderer_flush(renderer);

    const u32 shader_id = quad->shader_id != 0 ? quad->shader_id : renderer->default_shader;
    const u32 texture = quad->texture ? quad->texture : renderer->white_texture;

    const u32 q = renderer->quad_count;
    renderer->sort_keys[q] = renderer_make_key(shader_id, texture);
    renderer_write_quad_vertices(renderer, quad, q);

    ++renderer->quad_count;
    ++renderer->total_quads;
}

/** @brief Sort and draw all queued quads. */
void Renderer::end(Renderer* renderer)
{
    renderer_flush(renderer);
}

