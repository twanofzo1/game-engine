/**
 * @file renderer_batch.cpp
 * @brief CPU-side staging, sorting and draw-call batching for the renderer.
 *
 * Quads are grouped by (shader, texture) so that each contiguous group becomes
 * a single draw call. A least-significant-digit radix sort over the packed
 * 64-bit sort key keeps the sort O(8n) with no external sorting dependency.
 */
#include "common/graphics/rendering/renderer_internal.hpp"

#include <math.h>
#include <string.h>

/** Number of 8-bit digits in a 64-bit sort key. */
#define KEY_BYTES 8

/// @brief Packs a shader and texture into a single sortable 64-bit key.
/// @param shader_id Shader program name (stored in the high 32 bits).
/// @param texture Texture name (stored in the low 32 bits).
/// @return The packed key.
u64 renderer_make_key(const u32 shader_id, const u32 texture)
{
    return (static_cast<u64>(shader_id) << 32) | static_cast<u64>(texture);
}

/// @brief Extracts the shader_id from a packed sort key.
/// @param key The packed key.
/// @return The shader program name.
u32 renderer_key_shader(const u64 key)
{
    return static_cast<u32>(key >> 32);
}

/// @brief Extracts the texture from a packed sort key.
/// @param key The packed key.
/// @return The texture name.
u32 renderer_key_texture(const u64 key)
{
    return static_cast<u32>(key & 0xFFFFFFFFu);
}

/// @brief Writes the four vertices of a rotated quad into the staging buffer.
/// @param renderer The renderer owning the staging buffer.
/// @param quad The quad to write.
/// @param index The quad index to write at (vertices land at index * 4).
void renderer_write_quad_vertices(Renderer* const renderer, const QuadConfig* const quad, const u32 index)
{
    const f32 hw = quad->size.x * 0.5f;
    const f32 hh = quad->size.y * 0.5f;
    const f32 c  = cosf(quad->rotation);
    const f32 s  = sinf(quad->rotation);

    /* Local corners in counter-clockwise order: bottom-left, bottom-right,
     * top-right, top-left. */
    const f32 lx[VERTICES_PER_QUAD] = { -hw,  hw,  hw, -hw };
    const f32 ly[VERTICES_PER_QUAD] = { -hh, -hh,  hh,  hh };

    Vertex* const vertices = &renderer->vertices[index * VERTICES_PER_QUAD];
    for (u32 corner = 0; corner < VERTICES_PER_QUAD; ++corner)
    {
        const f32 rx = lx[corner] * c - ly[corner] * s;
        const f32 ry = lx[corner] * s + ly[corner] * c;

        vertices[corner].x = quad->position.x + rx;
        vertices[corner].y = quad->position.y + ry;
        vertices[corner].z = 0.0f;
        /* Corners: bl, br, tr, tl. u: left=u0, right=u1; v: top=v0, bottom=v1. */
        vertices[corner].u = (corner == 0 || corner == 3) ? quad->u0 : quad->u1;
        vertices[corner].v = (corner < 2) ? quad->v1 : quad->v0;
        vertices[corner].r = quad->color.r;
        vertices[corner].g = quad->color.g;
        vertices[corner].b = quad->color.b;
        vertices[corner].a = quad->color.a;
    }
}

/// @brief Counts and scatters into buckets for one 8-bit digit of the keys.
/// @param keys The list of keys to sort.
/// @param src The source permutation of indices.
/// @param dst The destination permutation receiving the scattered indices.
/// @param n The number of quads.
/// @param digit The byte offset (0 = least significant).
static void renderer_counting_pass(const u64* const keys, const u32* const src,
                                   u32* const dst, const u32 n, const u32 digit)
{
    u32 bucket[256];
    memset(bucket, 0, sizeof(bucket));

    const u32 shift = digit * 8;
    for (u32 i = 0; i < n; ++i)
    {
        const u8 d = static_cast<u8>((keys[src[i]] >> shift) & 0xFFu);
        ++bucket[d];
    }

    u32 cursor = 0;
    for (u32 b = 0; b < 256; ++b)
    {
        const u32 count = bucket[b];
        bucket[b] = cursor;
        cursor += count;
    }

    for (u32 i = 0; i < n; ++i)
    {
        const u8 d = static_cast<u8>((keys[src[i]] >> shift) & 0xFFu);
        dst[bucket[d]++] = src[i];
    }
}

/// @brief Radix-sorts quad indices by their packed sort key (LSD, 8-bit digits).
/// @param renderer The renderer holding the keys and the two permutations.
/// @param n The number of quads to sort.
static void renderer_radix_sort(Renderer* const renderer, const u32 n)
{
    u32* const order = renderer->order;
    u32* const tmp   = renderer->order_tmp;

    /* order starts as the identity permutation. */
    for (u32 i = 0; i < n; ++i)
        order[i] = i;

    /* Ping-pong between the two buffers across the 8 byte digits so that the
     * final result lands back in `order`. */
    for (u32 digit = 0; digit < KEY_BYTES; ++digit)
    {
        if ((digit & 1u) == 0u)
            renderer_counting_pass(renderer->sort_keys, order, tmp, n, digit);
        else
            renderer_counting_pass(renderer->sort_keys, tmp, order, n, digit);
    }
}

/// @brief Sorts quads by (shader, texture) key into the order permutation.
/// @param renderer The renderer holding the keys and order permutation.
void renderer_sort_keys(Renderer* const renderer)
{
    renderer_radix_sort(renderer, renderer->quad_count);
}

/// @brief Reorders the staging buffer to match the sorted quad order.
/// @param renderer The renderer holding the staging and scratch buffers.
void renderer_reorder_vertices(Renderer* const renderer)
{
    const u32 n = renderer->quad_count;
    memcpy(renderer->scratch, renderer->vertices,
           sizeof(Vertex) * n * VERTICES_PER_QUAD);
    for (u32 i = 0; i < n; ++i)
        memcpy(&renderer->vertices[i * VERTICES_PER_QUAD],
               &renderer->scratch[renderer->order[i] * VERTICES_PER_QUAD],
               sizeof(Vertex) * VERTICES_PER_QUAD);
}

/// @brief Draws all sorted quads, one draw call per (shader, texture) run.
/// @param renderer The renderer holding the sorted staging buffer.
void renderer_draw_sorted(Renderer* const renderer)
{
    const u32 n = renderer->quad_count;
    if (n == 0)
        return;

    u32 run_start  = 0;
    u32 run_shader = renderer_key_shader(renderer->sort_keys[renderer->order[0]]);
    u32 run_texture = renderer_key_texture(renderer->sort_keys[renderer->order[0]]);

    for (u32 i = 1; i <= n; ++i)
    {
        /* A run ends when the key changes or we run off the end. */
        bool boundary = (i == n);
        if (i < n)
        {
            const u64 key = renderer->sort_keys[renderer->order[i]];
            boundary = boundary
                    || renderer_key_shader(key)   != run_shader
                    || renderer_key_texture(key)  != run_texture;
        }

        if (boundary)
        {
            renderer->draw_run(run_shader, run_texture, i - run_start);
            run_start = i;
            if (i < n)
            {
                const u64 key = renderer->sort_keys[renderer->order[i]];
                run_shader  = renderer_key_shader(key);
                run_texture = renderer_key_texture(key);
            }
        }
    }
}

/// @brief Sorts and draws all queued quads, then resets the batch cursor.
/// @param renderer The renderer to flush.
void renderer_flush(Renderer* const renderer)
{
    if (renderer->quad_count == 0)
        return;

    renderer_sort_keys(renderer);
    renderer_reorder_vertices(renderer);
    renderer_draw_sorted(renderer);

    renderer->quad_count = 0;
}

