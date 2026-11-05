/**
 * @file test_memory_pool.cpp
 * @brief Tests for the fixed-size block memory pool.
 */
#include "framework.hpp"

#include "core/allocator/memory_pool.hpp"

#include <stddef.h>

void test_memory_pool(void)
{
    Engine::MemoryPool::Pool pool = Engine::MemoryPool::create(16, 4);

    /* All four blocks are available and distinct. */
    u32 a = Engine::MemoryPool::alloc_index(&pool);
    u32 b = Engine::MemoryPool::alloc_index(&pool);
    u32 c = Engine::MemoryPool::alloc_index(&pool);
    u32 d = Engine::MemoryPool::alloc_index(&pool);
    TEST_CHECK(a != b && a != c && a != d && b != c && b != d && c != d);

    /* Distinct blocks map to distinct addresses. */
    void* pa = Engine::MemoryPool::block(&pool, a);
    void* pb = Engine::MemoryPool::block(&pool, b);
    TEST_CHECK(pa != NULL && pb != NULL && pa != pb);

    /* A block holds data between writes. */
    *(u32*)pa = 0xDEADBEEFu;
    TEST_CHECK(*(u32*)pa == 0xDEADBEEFu);

    /* An exhausted pool returns the sentinel index. */
    TEST_CHECK(Engine::MemoryPool::alloc_index(&pool) == 0xFFFFFFFFu);

    /* Freeing a block makes it reusable. */
    Engine::MemoryPool::free_index(&pool, b);
    TEST_CHECK(Engine::MemoryPool::alloc_index(&pool) == b);

    /* grow() adds blocks with indices beyond the original allocation. */
    Engine::MemoryPool::grow(&pool, 4);
    u32 f = Engine::MemoryPool::alloc_index(&pool);
    TEST_CHECK(f != 0xFFFFFFFFu);
    TEST_CHECK(f >= 4);

    Engine::MemoryPool::destroy(&pool);
    TEST_CHECK(pool.storage == NULL);
    TEST_CHECK(pool.free_stack == NULL);
    TEST_CHECK(pool.block_count == 0);
}
