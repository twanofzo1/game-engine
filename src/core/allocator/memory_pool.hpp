/**
 * @file memory_pool.hpp
 * @brief Declares a fixed-size block memory pool with a free-list.
 *
 * The pool owns one contiguous allocation split into fixed-size blocks. Free
 * blocks are tracked by an index stack, so allocation and release are O(1)
 * and no per-block metadata is written into the blocks themselves. Block
 * contents therefore survive intact between release and reuse.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace MemoryPool {

/** A fixed-size block allocator backed by a free-index stack. */
struct Pool {
    byte* storage;     ///< Backing store: block_count * block_size bytes.
    u32*  free_stack;  ///< Stack of free block indices.
    u32   stack_top;   ///< Number of free blocks currently on the stack.
    u32   block_size;  ///< Size of one block in bytes.
    u32   block_count; ///< Total number of blocks.
};

/** @brief Create a pool of `block_count` blocks of `block_size` bytes each. */
Pool create(u32 block_size, u32 block_count);

/** @brief Release all memory owned by the pool. */
void destroy(Pool* pool);

/**
 * @brief Grow the pool by `additional_blocks` blocks.
 * @warning Reallocates the backing store; previously returned pointers are
 *          invalidated.
 */
void grow(Pool* pool, u32 additional_blocks);

/**
 * @brief Allocate one block.
 * @return Block index, or 0xFFFFFFFFu when the pool is exhausted.
 */
u32 alloc_index(Pool* pool);

/** @brief Release the block identified by `index`. */
void free_index(Pool* pool, u32 index);

/** @brief Return a pointer to the block identified by `index`. */
void* block(Pool* pool, u32 index);

} // namespace MemoryPool
} // namespace Engine
