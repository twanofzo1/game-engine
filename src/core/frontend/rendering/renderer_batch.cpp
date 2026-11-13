/**
 * @file renderer_batch.cpp
 * @brief CPU-side sorting and draw-call batching for the renderer.
 *
 * Quads are grouped by shader (and texture within a shader) so that each group
 * becomes a single draw call. A counting sort over shader_id keeps this O(n)
 * with no external sorting dependency.
 */
#include "core/frontend/rendering/renderer_internal.hpp"

#include <math.h>
#include <string.h>


u64 renderer_make_key(u32 shader_id, u32 texture)
{
    return ((u64)shader_id << 32) | (u64)texture;
}

u32 renderer_key_shader(u64 key)
{
    return (u32)(key >> 32);
}

u32 renderer_key_texture(u64 key)
{
    return (u32)(key & 0xFFFFFFFFu);
}

void renderer_write_quad_vertices(Renderer* renderer, const QuadConfig* quad, u32 index)
{
    const f32 hw = quad->size.x * 0.5f;
    const f32 hh = quad->size.y * 0.5f;
    const f32 c  = cosf(quad->rotation);
    const f32 s  = sinf(quad->rotation);

    /* Local corners in counter-clockwise order: bottom-left, bottom-right,
     * top-right, top-left. */
    const f32 lx[4] = { -hw,  hw,  hw, -hw };
    const f32 ly[4] = { -hh, -hh,  hh,  hh };

    Vertex* v = &renderer->vertices[index * 4];
    for (int i = 0; i < 4; ++i)
    {
        const f32 rx = lx[i] * c - ly[i] * s;
        const f32 ry = lx[i] * s + ly[i] * c;

        v[i].x = quad->position.x + rx;
        v[i].y = quad->position.y + ry;
        v[i].z = 0.0f;
        /* Corners: bl, br, tr, tl. u: left=u0, right=u1; v: top=v0, bottom=v1. */
        v[i].u = (i == 0 || i == 3) ? quad->u0 : quad->u1;
        v[i].v = (i < 2) ? quad->v1 : quad->v0;
        v[i].r = quad->color.r;
        v[i].g = quad->color.g;
        v[i].b = quad->color.b;
        v[i].a = quad->color.a;
    }
}

/**
 * @brief Sort quads into per-shader buckets, preserving submission order.
 *
 * A counting sort over shader_id (at most RENDERER_MAX_SHADERS buckets) groups
 * quads by shader in O(n + shaders) with no external dependencies.
 */
void renderer_sort_keys(Renderer* renderer)
{
    const u32 n = renderer->quad_count;

    u32 counts[RENDERER_MAX_SHADERS] = { 0 };
    for (u32 i = 0; i < n; ++i)
    {
        u32 sh = renderer_key_shader(renderer->sort_keys[i]);
        sh = sh < RENDERER_MAX_SHADERS ? sh : 0;
        ++counts[sh];
    }

    u32 offsets[RENDERER_MAX_SHADERS] = { 0 };
    u32 cursor = 0;
    for (u32 sh = 0; sh < RENDERER_MAX_SHADERS; ++sh)
    {
        offsets[sh] = cursor;
        cursor += counts[sh];
    }

    u32 next[RENDERER_MAX_SHADERS];
    memcpy(next, offsets, sizeof(next));
    for (u32 i = 0; i < n; ++i)
    {
        u32 sh = renderer_key_shader(renderer->sort_keys[i]);
        sh = sh < RENDERER_MAX_SHADERS ? sh : 0;
        renderer->order[next[sh]++] = i;
    }
}

void renderer_reorder_vertices(Renderer* renderer)
{
    const u32 n = renderer->quad_count;
    memcpy(renderer->scratch, renderer->vertices, sizeof(Vertex) * n * 4);
    for (u32 i = 0; i < n; ++i)
        memcpy(&renderer->vertices[i * 4], &renderer->scratch[renderer->order[i] * 4],
               sizeof(Vertex) * 4);
}

void renderer_draw_sorted(Renderer* renderer)
{
    const u32 n = renderer->quad_count;
    u32 run_start = 0;
    u32 run_shader = renderer_key_shader(renderer->sort_keys[renderer->order[0]]);
    u32 run_texture = renderer_key_texture(renderer->sort_keys[renderer->order[0]]);

    for (u32 i = 1; i <= n; ++i)
    {
        u32 sh = 0;
        u32 tx = 0;
        if (i < n)
        {
            sh = renderer_key_shader(renderer->sort_keys[renderer->order[i]]);
            tx = renderer_key_texture(renderer->sort_keys[renderer->order[i]]);
        }

        /* A run ends when the key changes or we run off the end. */
        const bool boundary = (i == n) || (sh != run_shader) || (tx != run_texture);
        if (boundary)
        {
            renderer_draw_run(renderer, run_shader, run_texture, i - run_start);
            run_start = i;
            run_shader = sh;
            run_texture = tx;
        }
    }
}

void renderer_flush(Renderer* renderer)
{
    if (renderer->quad_count == 0)
        return;

    renderer_sort_keys(renderer);
    renderer_reorder_vertices(renderer);
    renderer_draw_sorted(renderer);

    renderer->quad_count = 0;
}
