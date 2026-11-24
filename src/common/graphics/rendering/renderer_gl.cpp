/**
 * @file renderer_gl.cpp
 * @brief OpenGL resource creation and draw submission for the renderer.
 *
 * Owns the VAO/VBO/EBO setup, the 1x1 white texture, the vertex attribute
 * layout and the actual glDrawElements submission for a contiguous quad run.
 * The public entry points are Renderer member functions; the low-level GL glue
 * is private to this file.
 */
#include "common/graphics/rendering/renderer.hpp"
#include "common/graphics/shader/shader.hpp"
#include "common/graphics/camera/camera.hpp"

#include <glad/glad.h>

#include <cassert>
#include <stddef.h>

/// @brief Configuration for creating a 2D GL texture image.
struct GLTextureConfig {
    u32         index;
    GLenum      target;
    i32         level;
    i32         internal_format;
    i32         width;
    i32         height;
    i32         border;
    GLenum      format;
    GLenum      type;
    const void* data;
};

/// @brief Configuration for uploading data into a GL buffer object.
struct GLBufferConfig {
    u32        index;
    GLenum     target;
    GLsizeiptr size;
    const void* data;
    GLenum     usage;
};

/// @brief Configuration for one vertex attribute pointer.
struct GLVertexAttribPointerConfig {
    u32         index;
    i32         size;
    GLenum      type;
    GLboolean   normalized;
    GLsizei     stride;
    const void* pointer;
};

/// @brief Uploads a 2D texture image and applies repeat/linear sampling.
/// @param config The texture name, target, format and pixel data.
static void setup_gl_texture(const GLTextureConfig config)
{
    glBindTexture(config.target, config.index);
    glTexImage2D(config.target, config.level, config.internal_format,
                 config.width, config.height, config.border,
                 config.format, config.type, config.data);
    glTexParameteri(config.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(config.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(config.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(config.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(config.target, 0);
}

/// @brief Uploads data into a GL buffer object.
/// @param config The buffer name, target, size, data and usage hint.
static void setup_gl_buffer(const GLBufferConfig config)
{
    glBindBuffer(config.target, config.index);
    glBufferData(config.target, config.size, config.data, config.usage);
}

/// @brief Enables and describes one vertex attribute pointer.
/// @param config The attribute location, component layout and offset.
static void setup_gl_vertex_attrib_pointer(const GLVertexAttribPointerConfig config)
{
    glEnableVertexAttribArray(config.index);
    glVertexAttribPointer(
        config.index,
        config.size,
        config.type,
        config.normalized,
        config.stride,
        config.pointer);
}

/// @brief Configures all three vertex attribute pointers in the bound VAO.
static void setup_gl_vertex_attribs()
{
    setup_gl_vertex_attrib_pointer({
        .index = 0,
        .size = 3,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Vertex),
        .pointer = reinterpret_cast<void*>(offsetof(Vertex, x)),
    });

    setup_gl_vertex_attrib_pointer({
        .index = 1,
        .size = 2,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Vertex),
        .pointer = reinterpret_cast<void*>(offsetof(Vertex, u)),
    });

    setup_gl_vertex_attrib_pointer({
        .index = 2,
        .size = 4,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Vertex),
        .pointer = reinterpret_cast<void*>(offsetof(Vertex, r)),
    });
}

/// @brief Fills the index buffer with two triangles per quad.
/// @param indices Destination buffer for quad_count * INDICES_PER_QUAD indices.
/// @param quad_count Number of quads whose indices are written.
/// @warn indices must point to storage for quad_count * INDICES_PER_QUAD u32s.
static void fill_indices(u32* const indices, const u32 quad_count)
{
    assert(indices != nullptr);

    for (u32 q = 0; q < quad_count; ++q)
    {
        const u32 base = q * VERTICES_PER_QUAD;
        u32* const idx = &indices[q * INDICES_PER_QUAD];
        idx[0] = base + 0; idx[1] = base + 1; idx[2] = base + 2;
        idx[3] = base + 2; idx[4] = base + 3; idx[5] = base + 0;
    }
}

/// @brief Uploads the static index buffer (two triangles per quad).
/// @param renderer The renderer owning the EBO.
static void create_index_buffer(Renderer* const renderer)
{
    const usize index_bytes = sizeof(u32) * RENDERER_MAX_QUADS * INDICES_PER_QUAD;
    const std::expected<void*, ArenaError> alloc = renderer->arena.alloc(index_bytes);
    if (!alloc)
        return;

    u32* const indices = static_cast<u32*>(*alloc);
    fill_indices(indices, RENDERER_MAX_QUADS);
    setup_gl_buffer({
        .index = renderer->ebo,
        .target = GL_ELEMENT_ARRAY_BUFFER,
        .size = static_cast<GLsizeiptr>(index_bytes),
        .data = indices,
        .usage = GL_STATIC_DRAW,
    });
}

/// @brief Draws a contiguous run of quads with one shader and texture.
/// @param shader_id Shader program to draw with.
/// @param texture Texture to bind (white fallback for solid quads).
/// @param count Number of quads in the run.
void Renderer::draw_run(const u32 shader_id, const u32 texture, const u32 count)
{
    if (count == 0)
        return;

    Shader::bind(shader_id);

    /* The view-projection matrix is camera-only, so look it up by name rather
     * than caching a per-program location. */
    const ViewProjectionMatrix view_projection = this->camera.compute_orthographic_projection_matrix();
    Shader::set_mat4(shader_id, "uViewProj", view_projection.matrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(count * VERTICES_PER_QUAD * sizeof(Vertex)),
                    this->vertices);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count * INDICES_PER_QUAD),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

/// @brief Creates the 1x1 white texture used for solid-color quads.
void Renderer::create_white_texture()
{
    const u8 white_pixel[4] = { 255, 255, 255, 255 };
    setup_gl_texture({
        .index = this->white_texture,
        .target = GL_TEXTURE_2D,
        .level = 0,
        .internal_format = GL_RGBA8,
        .width = 1,
        .height = 1,
        .border = 0,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
        .data = white_pixel,
    });
}

/// @brief Creates the VAO and the dynamic/index buffers.
void Renderer::create_buffers()
{
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(Vertex) * RENDERER_MAX_QUADS * VERTICES_PER_QUAD),
                 nullptr, GL_DYNAMIC_DRAW);

    create_index_buffer(this);
    setup_gl_vertex_attribs();

    glBindVertexArray(0);
}