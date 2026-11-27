/**
 * @file renderer_gpu.cpp
 * @brief Shared GPU buffers, element pattern and white texture for the renderer.
 */
#include <cassert>

#include <glad/glad.h>

#include "common/graphics/rendering/gl_helper.hpp"
#include "common/graphics/rendering/renderer.hpp"
#include "common/graphics/rendering/renderer_gpu.hpp"
#include "common/graphics/rendering/vertex.hpp"

/** GPU buffers, VAO and white texture shared by every renderer batch. */
u32 g_vao = 0;
u32 g_vbo = 0;
u32 g_ebo = 0;
u32 g_white_texture = 0;

/// @brief Creates the 1x1 white texture used when a quad has no texture.
static void create_white_texture()
{
    const u8 white[4] = {255, 255, 255, 255};
    glGenTextures(1, &g_white_texture);
    glBindTexture(GL_TEXTURE_2D, g_white_texture);
    setup_gl_texture_image({
        .target = GL_TEXTURE_2D,
        .level = 0,
        .internal_format = GL_RGBA8,
        .width = 1,
        .height = 1,
        .border = 0,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
        .data = white,
    });
    setup_gl_tex_param({.target = GL_TEXTURE_2D, .param = GL_TEXTURE_MIN_FILTER, .value = GL_NEAREST});
    setup_gl_tex_param({.target = GL_TEXTURE_2D, .param = GL_TEXTURE_MAG_FILTER, .value = GL_NEAREST});
    glBindTexture(GL_TEXTURE_2D, 0);
}

/// @brief Creates the shared VAO/VBO/EBO used to submit vertex batches.
static void create_gpu_buffers()
{
    gl_gen_vertex_array({.output = &g_vao});
    gl_gen_buffer({.target = GL_ARRAY_BUFFER, .output = &g_vbo});
    gl_gen_buffer({.target = GL_ELEMENT_ARRAY_BUFFER, .output = &g_ebo});

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);

    const GLsizei stride = sizeof(Vertex);
    setup_gl_vertex_attrib_pointer({
        .index = 0,
        .size = 3,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = stride,
        .pointer = (const void*)offsetof(Vertex, x),
    });
    setup_gl_vertex_attrib_pointer({
        .index = 1,
        .size = 2,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = stride,
        .pointer = (const void*)offsetof(Vertex, u),
    });
    setup_gl_vertex_attrib_pointer({
        .index = 2,
        .size = 4,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = stride,
        .pointer = (const void*)offsetof(Vertex, r),
    });

    glBindVertexArray(0);
}

/// @brief Builds the element index pattern for the maximum quad count.
static void fill_index_buffer()
{
    const GLsizei max_indices = RENDERER_MAX_QUADS * INDICES_PER_QUAD;
    u32* const indices = new u32[max_indices];

    u32 cursor = 0;
    for (u32 i = 0; i < RENDERER_MAX_QUADS; ++i)
    {
        const u32 base = i * VERTICES_PER_QUAD;
        indices[cursor++] = base + 0;
        indices[cursor++] = base + 1;
        indices[cursor++] = base + 2;
        indices[cursor++] = base + 2;
        indices[cursor++] = base + 3;
        indices[cursor++] = base + 0;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
    gl_buffer_data({
        .target = GL_ELEMENT_ARRAY_BUFFER,
        .size = max_indices * (GLsizeiptr)sizeof(u32),
        .data = indices,
        .usage = GL_STATIC_DRAW,
    });
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] indices;
}

/// @brief Lazily creates the shared GPU buffers and white texture on first use.
void renderer_gpu_ensure_resources()
{
    if (g_vao != 0)
        return;

    create_white_texture();
    create_gpu_buffers();
    fill_index_buffer();
}
