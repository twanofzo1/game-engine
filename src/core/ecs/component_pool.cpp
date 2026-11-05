/**
 * @file component_pool.cpp
 * @brief Implementation of the sparse-set component storage.
 */
#include "core/ecs/component_pool.hpp"

#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Ecs {
namespace ComponentPool {

void init(ComponentPool* pool, u32 size,
                         u32 dense_capacity, u32 sparse_capacity)
{
    pool->size            = size;
    pool->blocks          = MemoryPool::create(size, dense_capacity);
    pool->sparse          = (u32*)calloc(sparse_capacity, sizeof(u32));
    pool->dense_entities  = (Entity*)malloc(sizeof(Entity) * dense_capacity);
    pool->dense_blocks    = (u32*)malloc(sizeof(u32) * dense_capacity);
    pool->count           = 0;
    pool->dense_capacity  = dense_capacity;
    pool->sparse_capacity = sparse_capacity;
}

void destroy(ComponentPool* pool)
{
    MemoryPool::destroy(&pool->blocks);
    free(pool->sparse);
    free(pool->dense_entities);
    free(pool->dense_blocks);
    pool->sparse          = NULL;
    pool->dense_entities  = NULL;
    pool->dense_blocks    = NULL;
    pool->count           = 0;
    pool->dense_capacity  = 0;
    pool->sparse_capacity = 0;
}

void grow_sparse(ComponentPool* pool, u32 sparse_capacity)
{
    if (sparse_capacity <= pool->sparse_capacity)
        return;

    /* @warning: raw realloc used for the sparse map; growth is rare, so the
     *            cost of copying is acceptable. */
    pool->sparse = (u32*)realloc(pool->sparse, sizeof(u32) * sparse_capacity);
    memset(pool->sparse + pool->sparse_capacity, 0,
           sizeof(u32) * (sparse_capacity - pool->sparse_capacity));
    pool->sparse_capacity = sparse_capacity;
}

void grow_dense(ComponentPool* pool, u32 dense_capacity)
{
    if (dense_capacity <= pool->dense_capacity)
        return;

    /* @warning: raw realloc used for the dense arrays; growth is rare. */
    pool->dense_entities = (Entity*)realloc(pool->dense_entities, sizeof(Entity) * dense_capacity);
    pool->dense_blocks   = (u32*)realloc(pool->dense_blocks, sizeof(u32) * dense_capacity);
    pool->dense_capacity = dense_capacity;

    if (pool->blocks.block_count < dense_capacity)
        MemoryPool::grow(&pool->blocks, dense_capacity - pool->blocks.block_count);
}

void* add(ComponentPool* pool, Entity entity)
{
    u32 index = entity.id;
    if (pool->sparse[index] != 0)
        return get(pool, entity);

    if (pool->count == pool->dense_capacity)
        grow_dense(pool, pool->dense_capacity == 0 ? 64 : pool->dense_capacity * 2);

    u32 block_index = MemoryPool::alloc_index(&pool->blocks);
    u32 dense       = pool->count;

    pool->dense_blocks[dense]   = block_index;
    pool->dense_entities[dense] = entity;
    pool->sparse[index]         = dense + 1;
    ++pool->count;

    return MemoryPool::block(&pool->blocks, block_index);
}

void remove(ComponentPool* pool, Entity entity)
{
    u32 index = entity.id;
    u32 dense = pool->sparse[index];
    if (dense == 0)
        return;
    --dense;

    u32 last = pool->count - 1;

    /* Swap the last live component into the removed slot, then release the
     * vacated block. */
    void* dst = MemoryPool::block(&pool->blocks, pool->dense_blocks[dense]);
    void* src = MemoryPool::block(&pool->blocks, pool->dense_blocks[last]);
    if (dst != src)
        memcpy(dst, src, pool->size);

    Entity moved = pool->dense_entities[last];
    pool->dense_entities[dense] = moved;
    pool->sparse[moved.id] = dense + 1;

    MemoryPool::free_index(&pool->blocks, pool->dense_blocks[last]);
    pool->sparse[index] = 0;
    --pool->count;
}

void* get(ComponentPool* pool, Entity entity)
{
    u32 index = entity.id;
    u32 dense = pool->sparse[index];
    if (dense == 0)
        return NULL;
    return MemoryPool::block(&pool->blocks, pool->dense_blocks[dense - 1]);
}

} // namespace ComponentPool
} // namespace Ecs
} // namespace Engine
