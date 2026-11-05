/**
 * @file renderer.cpp
 * @brief 2D batch renderer implementation.
 *
 * Draws rotated, solid-color quads with one dynamic vertex buffer. Quads are
 * staged in a CPU buffer and submitted in fixed-size batches: when a batch
 * fills up it is flushed with a draw call and a new batch is started.
 */
#include "graphics/rendering/renderer.hpp"
#include "graphics/rendering/shader.hpp"
#include "graphics/rendering/camera.hpp"

#include <glad/glad.h>

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

namespace Engine {
namespace Render {

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

void renderer_init(Renderer* renderer)
{
    renderer->quad_count = 0;
    renderer->total_quads = 0;
    renderer->camera = camera_make(Vec2::make(0.0f, 0.0f), Vec2::make(1280.0f, 720.0f));

    renderer->vertices = (Vertex*)malloc(sizeof(Vertex) * RENDERER_MAX_QUADS * 4);

    renderer->program = shader_create(VERT_SRC, FRAG_SRC);
    renderer->u_view_proj = glGetUniformLocation(renderer->program, "uViewProj");
    renderer->u_texture = glGetUniformLocation(renderer->program, "uTexture");

    /* Bind the color texture sampler to texture unit 0 once. */
    shader_bind(renderer->program);
    glUniform1i(renderer->u_texture, 0);

    /* 1x1 white texture used for solid-color (untextured) quads. */
    const u8 white[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &renderer->white_texture);
    glBindTexture(GL_TEXTURE_2D, renderer->white_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderer->current_texture = renderer->white_texture;

    /* Alpha blending so transparent textures composite over the background. */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &renderer->vao);
    glGenBuffers(1, &renderer->vbo);
    glGenBuffers(1, &renderer->ebo);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(sizeof(Vertex) * RENDERER_MAX_QUADS * 4),
                 NULL, GL_DYNAMIC_DRAW);

    /* Static index buffer: every quad is two triangles (0,1,2, 2,3,0). */
    u32* indices = (u32*)malloc(sizeof(u32) * RENDERER_MAX_QUADS * 6);
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
    free(indices);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, r));

    glBindVertexArray(0);
}

void renderer_shutdown(Renderer* renderer)
{
    shader_destroy(renderer->program);
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteBuffers(1, &renderer->ebo);
    glDeleteTextures(1, &renderer->white_texture);
    free(renderer->vertices);
    renderer->vertices = NULL;
}

void renderer_begin(Renderer* renderer, const Camera* camera)
{
    renderer->camera = *camera;
    renderer->quad_count = 0;
    renderer->total_quads = 0;
    renderer->current_texture = renderer->white_texture;
}

/**
 * @brief Upload and draw the current batch, then reset it for the next one.
 */
static void flush(Renderer* renderer)
{
    if (renderer->quad_count == 0)
        return;

    shader_bind(renderer->program);

    f32 m[16];
    camera_view_projection(&renderer->camera, m);
    glUniformMatrix4fv(renderer->u_view_proj, 1, GL_FALSE, m);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->current_texture);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(renderer->quad_count * 4 * sizeof(Vertex)),
                    renderer->vertices);
    glDrawElements(GL_TRIANGLES, (GLsizei)(renderer->quad_count * 6),
                   GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    renderer->quad_count = 0;
}

void renderer_push_quad(Renderer* renderer, const Ecs::Transform* transform, const Ecs::Sprite* sprite)
{
    u32 texture = sprite->texture ? sprite->texture : renderer->white_texture;
    if (texture != renderer->current_texture)
    {
        flush(renderer);
        renderer->current_texture = texture;
    }

    if (renderer->quad_count >= RENDERER_MAX_QUADS)
        flush(renderer); /* batch full: draw it and start a new one. */

    f32 hw = sprite->size.x * 0.5f * transform->scale.x;
    f32 hh = sprite->size.y * 0.5f * transform->scale.y;
    f32 c  = cosf(transform->rotation);
    f32 s  = sinf(transform->rotation);

    /* Local corners in counter-clockwise order: bottom-left, bottom-right,
     * top-right, top-left. */
    const f32 lx[4] = { -hw,  hw,  hw, -hw };
    const f32 ly[4] = { -hh, -hh,  hh,  hh };

    /* UV rectangle of the current frame: [u0, v0, u1, v1]. */
    f32 uv[4];
    Sprite::uv_rect(sprite, uv);

    Vertex* v = &renderer->vertices[renderer->quad_count * 4];
    for (int i = 0; i < 4; ++i)
    {
        f32 rx = lx[i] * c - ly[i] * s;
        f32 ry = lx[i] * s + ly[i] * c;

        v[i].x = transform->position.x + rx;
        v[i].y = transform->position.y + ry;
        v[i].z = 0.0f;
        /* Corners: bl, br, tr, tl. u: left=u0, right=u1; v: top=v0, bottom=v1. */
        v[i].u = (i == 0 || i == 3) ? uv[0] : uv[2];
        v[i].v = (i < 2) ? uv[3] : uv[1];
        v[i].r = sprite->color.r;
        v[i].g = sprite->color.g;
        v[i].b = sprite->color.b;
        v[i].a = sprite->color.a;
    }

    ++renderer->quad_count;
    ++renderer->total_quads;
}

void renderer_end(Renderer* renderer)
{
    flush(renderer);
}

} // namespace Render
} // namespace Engine
