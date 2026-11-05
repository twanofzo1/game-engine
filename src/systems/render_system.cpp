/**
 * @file render_system.cpp
 * @brief Sprite rendering system: sorts sprites by z, then queues quads.
 *
 * Sprites are drawn back-to-front (painter's algorithm) so a higher `z` value
 * ends up on top. The hot loop reaches into the sparse-set pools directly (the
 * World struct is plain data, and the pools are designed for direct iteration),
 * which avoids the per-entity liveness check and one sparse lookup that the
 * generic accessors perform on every sprite.
 */
#include "systems/render_system.hpp"

#include "core/allocator/memory_pool.hpp"

#include <stdlib.h>

namespace Engine {
namespace Systems {

/** @brief One drawable sprite plus its sort key. */
struct SpriteEntry {
    f32 z;       ///< Draw order: higher z is drawn on top.
    u32 texture; ///< Raw texture id (0 = solid color), for stable batching.
    u32 index;   ///< Dense index of the sprite in the sprite pool.
};

/**
 * @brief Compare sprites back-to-front: lower z first, then texture for batching.
 */
static int compare_sprite_entries(const void* a, const void* b)
{
    const SpriteEntry* ea = (const SpriteEntry*)a;
    const SpriteEntry* eb = (const SpriteEntry*)b;

    if (ea->z < eb->z) return -1;
    if (ea->z > eb->z) return  1;
    if (ea->texture < eb->texture) return -1;
    if (ea->texture > eb->texture) return  1;
    return 0;
}

void render_system_draw(Ecs::World* world, Render::Renderer* renderer)
{
    Ecs::ComponentPool::ComponentPool* sprites    = &world->pools[Ecs::COMPONENT_SPRITE];
    Ecs::ComponentPool::ComponentPool* transforms = &world->pools[Ecs::COMPONENT_TRANSFORM];

    u32 count = sprites->count;
    if (count == 0)
        return;

    /* Scratch buffer reused across frames; grown on demand. */
    static SpriteEntry* entries        = NULL;
    static u32          entry_capacity = 0;

    if (entry_capacity < count)
    {
        free(entries);
        entry_capacity = count;
        entries = (SpriteEntry*)malloc(sizeof(SpriteEntry) * entry_capacity);
    }

    /* @warning: iterates the pools directly and skips liveness checks; valid
     *           because dense entities are always live while the world is
     *           intact, and much faster than the generic accessors. */
    u32 n = 0;
    for (u32 i = 0; i < count; ++i)
    {
        Ecs::Entity entity = sprites->dense_entities[i];

        /* sparse[id] is the dense index + 1; 0 means "no transform". */
        u32 t = transforms->sparse[entity.id];
        if (t == 0)
            continue;

        const Ecs::Sprite* sprite = (const Ecs::Sprite*)MemoryPool::block(
            &sprites->blocks, sprites->dense_blocks[i]);

        entries[n].z       = sprite->z;
        entries[n].texture = sprite->texture;
        entries[n].index   = i;
        ++n;
    }

    /* Painter's algorithm: draw back-to-front so higher z ends up on top.
     * Ties are grouped by texture to minimize renderer batch flushes. */
    qsort(entries, n, sizeof(SpriteEntry), compare_sprite_entries);

    for (u32 k = 0; k < n; ++k)
    {
        u32 i = entries[k].index;
        Ecs::Entity entity = sprites->dense_entities[i];

        Ecs::Transform transform;
        if (!Ecs::world_get_transform(world, entity, &transform))
            continue;

        const Ecs::Sprite* sprite = (const Ecs::Sprite*)MemoryPool::block(
            &sprites->blocks, sprites->dense_blocks[i]);

        Render::renderer_push_quad(renderer, &transform, sprite);
    }
}

} // namespace Systems
} // namespace Engine
