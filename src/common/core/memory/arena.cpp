/**
 * @file arena.cpp
 * @brief Virtual-memory-backed linear (bump) allocator implementation.
 *
 * A large virtual address range is reserved up front and physical pages are
 * committed on demand in page-sized chunks. Windows uses VirtualAlloc/
 * VirtualFree; POSIX uses mmap/mprotect/munmap.
 */
#include "common/core/memory/arena.hpp"

#include <string.h>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif


/** @brief The OS page size for the current process. */
static usize page_size()
{
#if defined(_WIN32)
    static usize cached = 0;
    if (!cached)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        cached = (usize)info.dwPageSize;
    }
    return cached;
#else
    return (usize)sysconf(_SC_PAGESIZE);
#endif
}

/** @brief Round a byte count up to a multiple of the OS page size. */
static usize round_page(usize bytes)
{
    const usize ps = page_size();
    return (bytes + ps - 1) & ~(ps - 1);
}

/** @brief Round a byte offset up to the given power-of-two alignment. */
static usize align_up(usize offset, usize alignment)
{
    return (offset + alignment - 1) & ~(alignment - 1);
}

/** @brief True when `value` is a power of two. */
static bool is_power_of_two(usize value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

/** @brief Reserve `bytes` of virtual address space, or nullptr on failure. */
static void* reserve_memory(usize bytes)
{
#if defined(_WIN32)
    return VirtualAlloc(NULL, bytes, MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(NULL, bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? nullptr : ptr;
#endif
}

/** @brief Commit `bytes` of previously reserved memory starting at `base`. */
static bool commit_memory(void* base, usize bytes)
{
#if defined(_WIN32)
    return VirtualAlloc(base, bytes, MEM_COMMIT, PAGE_READWRITE) == base;
#else
    return mprotect(base, bytes, PROT_READ | PROT_WRITE) == 0;
#endif
}

/** @brief Decommit `bytes` of memory starting at `base`, keeping it reserved. */
static void decommit_memory(void* base, usize bytes)
{
#if defined(_WIN32)
    VirtualFree(base, bytes, MEM_DECOMMIT);
#else
    mprotect(base, bytes, PROT_NONE);
#endif
}

/** @brief Release the entire reserved address range. */
static void release_memory(void* base, usize reserved)
{
    if (!base)
        return;
#if defined(_WIN32)
    (void)reserved;
    VirtualFree(base, 0, MEM_RELEASE);
#else
    munmap(base, reserved);
#endif
}

/**
 * @brief Grow the committed region so it covers at least `needed` bytes.
 * @return Success, or CommitFailed/OutOfReserved if it cannot be satisfied.
 */
static ArenaError ensure_committed(Arena* arena, usize needed)
{
    if (needed <= arena->committed)
        return ArenaError::Success;

    if (needed > arena->reserved)
        return ArenaError::OutOfReserved;

    const usize target = round_page(needed);
    const usize base_offset = round_page(arena->committed);
    const usize grow_bytes = target - base_offset;

    if (grow_bytes > 0 && !commit_memory(arena->buffer + base_offset, grow_bytes))
        return ArenaError::CommitFailed;

    arena->committed = target;
    return ArenaError::Success;
}

std::expected<Arena, ArenaError> Arena::make(const ArenaConfig config)
{
    if (config.reserved == 0 || config.commit > config.reserved)
        return std::unexpected(ArenaError::InvalidConfig);

    u8* buffer = (u8*)reserve_memory(config.reserved);
    if (!buffer)
        return std::unexpected(ArenaError::ReserveFailed);

    Arena arena;
    arena.buffer       = buffer;
    arena.reserved     = config.reserved;
    arena.committed    = 0;
    arena.used         = 0;
    arena.commit_floor = round_page(config.commit);

    const ArenaError err = ensure_committed(&arena, arena.commit_floor);
    if (err != ArenaError::Success)
    {
        release_memory(buffer, config.reserved);
        return std::unexpected(err);
    }

    return arena;
}

void Arena::destroy(Arena* arena)
{
    release_memory(arena->buffer, arena->reserved);
    arena->buffer       = nullptr;
    arena->reserved     = 0;
    arena->committed    = 0;
    arena->used         = 0;
    arena->commit_floor = 0;
}

/** @brief Bump-allocate `bytes` with `alignment`, committing pages as needed. */
static std::expected<void*, ArenaError> arena_bump(Arena* arena, usize bytes, usize alignment)
{
    if (!is_power_of_two(alignment))
        return std::unexpected(ArenaError::InvalidAlignment);

    const usize aligned = align_up(arena->used, alignment);
    const usize end = aligned + bytes;

    const ArenaError err = ensure_committed(arena, end);
    if (err != ArenaError::Success)
        return std::unexpected(err);

    void* ptr = arena->buffer + aligned;
    arena->used = end;
    return ptr;
}

std::expected<void*, ArenaError> Arena::alloc(Arena* arena, usize bytes)
{
    return arena_bump(arena, bytes, 1);
}

std::expected<void*, ArenaError> Arena::alloc_aligned(Arena* arena, usize bytes, usize alignment)
{
    return arena_bump(arena, bytes, alignment);
}

std::expected<void*, ArenaError> Arena::alloc_zeroed(Arena* arena, usize bytes)
{
    const std::expected<void*, ArenaError> ptr = arena_bump(arena, bytes, 1);
    if (ptr)
        memset(*ptr, 0, bytes);
    return ptr;
}

std::expected<void*, ArenaError> Arena::alloc_zeroed_aligned(Arena* arena, usize bytes, usize alignment)
{
    const std::expected<void*, ArenaError> ptr = arena_bump(arena, bytes, alignment);
    if (ptr)
        memset(*ptr, 0, bytes);
    return ptr;
}

void Arena::reset(Arena* arena)
{
    arena->used = 0;

    const usize floor = arena->commit_floor;
    if (arena->committed > floor)
    {
        decommit_memory(arena->buffer + floor, arena->committed - floor);
        arena->committed = floor;
    }
}

ArenaError Arena::pop(Arena* arena, usize bytes)
{
    if (bytes > arena->used)
    {
        arena->used = 0;
        return ArenaError::PoppedPastEnd;
    }

    arena->used -= bytes;
    return ArenaError::Success;
}
