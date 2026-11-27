/**
 * @file renderer.cpp
 * @brief Batched quad renderer implementation.
 */
#include <algorithm>
#include <cassert>

#include <glad/glad.h>

#include "common/graphics/rendering/gl_helper.hpp"
#include "common/graphics/rendering/renderer.hpp"
#include "common/graphics/rendering/renderer_gpu.hpp"
#include "common/log/log.hpp"

/// @brief Writes one quad's four corners into a vertex array.
/// @param config The quad geometry, color, texture and draw order.
/// @param out The destination vertex array (4 corners).
static void build_quad(const QuadConfig config, Vertex* const out)
{
    const f32 x0 = config.position.x;
    const f32 y0 = config.position.y;
    const f32 x1 = config.position.x + config.size.x;
    const f32 y1 = config.position.y + config.size.y;

    out[0] = {x0, y0, 0.0f, config.uv_min.x, config.uv_max.y, config.color.r, config.color.g, config.color.b, config.color.a};
    out[1] = {x1, y0, 0.0f, config.uv_max.x, config.uv_max.y, config.color.r, config.color.g, config.color.b, config.color.a};
    out[2] = {x1, y1, 0.0f, config.uv_max.x, config.uv_min.y, config.color.r, config.color.g, config.color.b, config.color.a};
    out[3] = {x0, y1, 0.0f, config.uv_min.x, config.uv_min.y, config.color.r, config.color.g, config.color.b, config.color.a};
}

/// @brief Orders quads back-to-front by z, grouping by shader then texture.
/// @param a The first quad.
/// @param b The second quad.
/// @return True when a should draw before b.
static bool quad_sort(const Quad a, const Quad b)
{
    if (a.z != b.z)
        return a.z < b.z;
    if (a.shader != b.shader)
        return a.shader < b.shader;
    return a.texture < b.texture;
}

/// @brief Flushes all buffered quads to the GPU in one or more draw calls.
/// @param renderer The renderer holding the buffered quads.
static void flush(Renderer* const renderer)
{
    if (renderer->quads.empty())
        return;

    renderer_gpu_ensure_resources();
    std::sort(renderer->quads.begin(), renderer->quads.end(), quad_sort);

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

    usize drawn = 0;
    while (drawn < renderer->quads.size())
    {
        // Find the contiguous run sharing the same shader + texture.
        const u32 shader = renderer->quads[drawn].shader;
        const u32 texture = renderer->quads[drawn].texture;
        usize run_end = drawn + 1;
        while (run_end < renderer->quads.size()
            && renderer->quads[run_end].shader == shader
            && renderer->quads[run_end].texture == texture)
        {
            ++run_end;
        }

        const usize quad_count = run_end - drawn;
        const usize vertex_count = quad_count * VERTICES_PER_QUAD;
        const usize index_count = quad_count * INDICES_PER_QUAD;

        Vertex* const vertices = new Vertex[vertex_count];
        for (usize q = 0; q < quad_count; ++q)
        {
            for (u32 v = 0; v < VERTICES_PER_QUAD; ++v)
            {
                vertices[q * VERTICES_PER_QUAD + v] = renderer->quads[drawn + q].vertices[v];
            }
        }

        gl_buffer_data({
            .target = GL_ARRAY_BUFFER,
            .size = (GLsizeiptr)(vertex_count * sizeof(Vertex)),
            .data = vertices,
            .usage = GL_DYNAMIC_DRAW,
        });
        delete[] vertices;

        glUseProgram(shader);

        const u32 tex = texture != 0 ? texture : g_white_texture;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        gl_draw_elements({
            .mode = GL_TRIANGLES,
            .count = (GLsizei)index_count,
            .type = GL_UNSIGNED_INT,
            .indices = 0,
        });

        drawn = run_end;
    }

    glBindVertexArray(0);
    renderer->quads.clear();
}

/// @brief Creates a renderer with the given default quad shader.
/// @param default_shader The program used when a quad omits `shader`.
/// @return The renderer.
Renderer Renderer::create(const u32 default_shader)
{
    Renderer renderer;
    renderer.default_shader_ = default_shader;
    renderer.active_ = false;
    return renderer;
}

/// @brief Starts a new batch.
void Renderer::begin()
{
    this->quads.clear();
    this->active_ = true;
}

/// @brief Buffers one quad into the current batch.
/// @param config The quad geometry, color, texture, shader and z order.
void Renderer::push_quad(const QuadConfig config)
{
    if (!this->active_)
        return;

    Quad quad;
    quad.shader = config.shader != 0 ? config.shader : this->default_shader_;
    quad.texture = config.texture;
    quad.z = config.z;
    build_quad(config, quad.vertices);
    this->quads.push_back(quad);
}

/// @brief Ends the current batch and flushes it to the GPU.
void Renderer::end()
{
    this->active_ = false;
    flush(this);
}
