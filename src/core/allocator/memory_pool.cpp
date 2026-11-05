/**
 * @file memory_pool.cpp
 * @brief Implementation of the fixed-size block memory pool.
 */
#include "core/allocator/memory_pool.hpp"

#include <stdlib.h>

namespace Engine {
namespace MemoryPool {

Pool create(u32 block_size, u32 block_count)
{
    Pool pool;
    pool.block_size  = block_size;
    pool.block_count = block_count;
    pool.stack_top   = 0;
    pool.storage     = (u8*)malloc((size_t)block_size * block_count);
    pool.free_stack  = (u32*)malloc(sizeof(u32) * block_count);

    for (u32 i = 0; i < block_count; ++i)
        pool.free_stack[pool.stack_top++] = i;

    return pool;
}

void destroy(Pool* pool)
{
    free(pool->storage);
    free(pool->free_stack);
    pool->storage     = NULL;
    pool->free_stack  = NULL;
    pool->block_count = 0;
    pool->stack_top   = 0;
}

void grow(Pool* pool, u32 additional_blocks)
{
    u32 old_count = pool->block_count;
    u32 new_count = old_count + additional_blocks;

    pool->storage = (byte*)realloc(pool->storage, (size_t)pool->block_size * new_count);
    pool->free_stack = (u32*)realloc(pool->free_stack, sizeof(u32) * new_count);

    for (u32 i = old_count; i < new_count; ++i)
        pool->free_stack[pool->stack_top++] = i;

    pool->block_count = new_count;
}

u32 alloc_index(Pool* pool)
{
    if (pool->stack_top == 0)
        return 0xFFFFFFFFu; /* @warning: pool exhausted; caller must grow it. */
    return pool->free_stack[--pool->stack_top];
}

void free_index(Pool* pool, u32 index)
{
    pool->free_stack[pool->stack_top++] = index;
}

void* block(Pool* pool, u32 index)
{
    return pool->storage + (size_t)index * pool->block_size;
}

} // namespace MemoryPool
} // namespace Engine
