/**
 * @file renderer_gl.cpp
 * @brief OpenGL resource creation and draw submission for the renderer.
 *
 * Owns the VAO/VBO/EBO setup, the 1x1 white texture, the vertex attribute
 * layout and the actual glDrawElements submission for a contiguous quad run.
 */
#include "core/frontend/rendering/renderer_internal.hpp"
#include "core/frontend/rendering/shader.hpp"
#include "core/frontend/camera/camera.hpp"

#include <glad/glad.h>

#include <stddef.h>


void renderer_draw_run(Renderer* renderer, u32 shader_id, u32 texture, u32 count)
{
    if (count == 0)
        return;

    Shader::bind(shader_id);

    /* The view-projection matrix is camera-only, so look it up by name rather
     * than caching a per-program location. */
    f32 m[16];
    Camera::view_projection(&renderer->camera, m);
    Shader::set_mat4(shader_id, "uViewProj", m);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(count * 4 * sizeof(Vertex)),
                    renderer->vertices);
    glDrawElements(GL_TRIANGLES, (GLsizei)(count * 6),
                   GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void renderer_create_white_texture(Renderer* renderer)
{
    const u8 white[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &renderer->white_texture);
    glBindTexture(GL_TEXTURE_2D, renderer->white_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/** @brief Upload the static index buffer (two triangles per quad). */
static void renderer_create_index_buffer(Renderer* renderer)
{
    const std::expected<void*, ArenaError> alloc = Arena::alloc(&renderer->arena, sizeof(u32) * RENDERER_MAX_QUADS * 6);
    if (!alloc)
        return;

    u32* indices = (u32*)*alloc;
    for (u32 q = 0; q < RENDERER_MAX_QUADS; ++q)
    {
        u32 base = q * 4;
        u32* idx = &indices[q * 6];
        idx[0] = base + 0; idx[1] = base + 1; idx[2] = base + 2;
        idx[3] = base + 2; idx[4] = base + 3; idx[5] = base + 0;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(sizeof(u32) * RENDERER_MAX_QUADS * 6),
                 indices, GL_STATIC_DRAW);
}

/** @brief Configure the vertex attribute layout in the VAO. */
static void renderer_setup_vertex_attribs()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, r));
}

void renderer_create_buffers(Renderer* renderer)
{
    glGenVertexArrays(1, &renderer->vao);
    glGenBuffers(1, &renderer->vbo);
    glGenBuffers(1, &renderer->ebo);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(sizeof(Vertex) * RENDERER_MAX_QUADS * 4),
                 NULL, GL_DYNAMIC_DRAW);

    renderer_create_index_buffer(renderer);
    renderer_setup_vertex_attribs();

    glBindVertexArray(0);
}
