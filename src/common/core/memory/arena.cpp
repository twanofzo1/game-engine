/**
 * @file arena.cpp
 * @brief Virtual-memory-backed linear (bump) allocator implementation.
 *
 * A large virtual address range is reserved up front and physical pages are
 * committed on demand in page-sized chunks. Windows uses VirtualAlloc/
 * VirtualFree; POSIX uses mmap/mprotect/munmap.
 */
#include "common/core/memory/arena.hpp"
#include "common/log/log.hpp"

#include <string.h>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif


/// @brief Returns the OS page size for the current process.
/// @return The page size in bytes.
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

/// @brief Rounds a byte count up to a multiple of the OS page size.
/// @param bytes The byte count.
/// @return The rounded byte count.
static usize round_page(const usize bytes)
{
    const usize ps = page_size();
    return (bytes + ps - 1) & ~(ps - 1);
}

/// @brief Rounds a byte offset up to the given power-of-two alignment.
/// @param offset The byte offset.
/// @param alignment The alignment (must be a power of two).
/// @return The aligned offset.
static usize align_up(const usize offset, const usize alignment)
{
    return (offset + alignment - 1) & ~(alignment - 1);
}

/// @brief Returns true when `value` is a power of two.
/// @param value The value to test.
/// @return True when the value is a power of two.
static bool is_power_of_two(const usize value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

/// @brief Reserves `bytes` of virtual address space.
/// @param bytes The number of bytes to reserve.
/// @return The base address, or nullptr on failure.
static void* reserve_memory(const usize bytes)
{
#if defined(_WIN32)
    return VirtualAlloc(NULL, bytes, MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(NULL, bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? nullptr : ptr;
#endif
}

/// @brief Commits `bytes` of previously reserved memory starting at `base`.
/// @param base The base address of the reserved range.
/// @param bytes The number of bytes to commit.
/// @return True on success.
static bool commit_memory(void* const base, const usize bytes)
{
#if defined(_WIN32)
    return VirtualAlloc(base, bytes, MEM_COMMIT, PAGE_READWRITE) == base;
#else
    return mprotect(base, bytes, PROT_READ | PROT_WRITE) == 0;
#endif
}

/// @brief Decommits `bytes` of memory starting at `base`, keeping it reserved.
/// @param base The base address.
/// @param bytes The number of bytes to decommit.
static void decommit_memory(void* const base, const usize bytes)
{
#if defined(_WIN32)
    VirtualFree(base, bytes, MEM_DECOMMIT);
#else
    mprotect(base, bytes, PROT_NONE);
#endif
}

/// @brief Releases the entire reserved address range.
/// @param base The base address of the reserved range.
/// @param reserved The total reserved byte count.
static void release_memory(void* const base, const usize reserved)
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

/// @brief Grows the committed region so it covers at least `needed` bytes.
/// @param arena The arena to grow.
/// @param needed The minimum number of committed bytes required.
/// @return Success, or CommitFailed/OutOfReserved when it cannot be satisfied.
static ArenaError ensure_committed(Arena* const arena, const usize needed)
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

/// @brief Reserves an address range and commits the configured initial bytes.
/// @param config The reserved size and up-front commit size.
/// @return The arena, or an error when reservation or commit fails.
std::expected<Arena, ArenaError> Arena::make(const ArenaConfig config)
{
    log_debug(LOG_SECTION_CORE, "arena make reserved=%zu commit=%zu", config.reserved, config.commit);
    if (config.reserved == 0 || config.commit > config.reserved)
        return std::unexpected(ArenaError::InvalidConfig);

    u8* const buffer = (u8*)reserve_memory(config.reserved);
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

    log_debug(LOG_SECTION_CORE, "arena ready (buffer=%p)", (void*)buffer);
    return arena;
}

/// @brief Frees the reserved address range and resets all state.
void Arena::destroy()
{
    log_debug(LOG_SECTION_CORE, "arena destroy (used=%zu)", this->used);
    release_memory(this->buffer, this->reserved);
    this->buffer       = nullptr;
    this->reserved     = 0;
    this->committed    = 0;
    this->used         = 0;
    this->commit_floor = 0;
}

/// @brief Bump-allocates `bytes` with `alignment`, committing pages as needed.
/// @param arena The arena to allocate from.
/// @param bytes The number of bytes to allocate.
/// @param alignment The alignment (must be a power of two).
/// @return The allocated pointer, or an error.
static std::expected<void*, ArenaError> arena_bump(Arena* const arena, const usize bytes, const usize alignment)
{
    if (!is_power_of_two(alignment))
        return std::unexpected(ArenaError::InvalidAlignment);

    const usize aligned = align_up(arena->used, alignment);
    const usize end = aligned + bytes;

    const ArenaError err = ensure_committed(arena, end);
    if (err != ArenaError::Success)
        return std::unexpected(err);

    void* const ptr = arena->buffer + aligned;
    arena->used = end;
    return ptr;
}

/// @brief Allocates a block of memory from the arena.
/// @param bytes The number of bytes to allocate.
/// @return The allocated pointer, or an error.
std::expected<void*, ArenaError> Arena::alloc(const usize bytes)
{
    return arena_bump(this, bytes, 1);
}

/// @brief Allocates an aligned block of memory from the arena.
/// @param bytes The number of bytes to allocate.
/// @param alignment The alignment (must be a power of two).
/// @return The allocated pointer, or an error.
std::expected<void*, ArenaError> Arena::alloc_aligned(const usize bytes, const usize alignment)
{
    return arena_bump(this, bytes, alignment);
}

/// @brief Allocates a zeroed block of memory from the arena.
/// @param bytes The number of bytes to allocate.
/// @return The allocated pointer, or an error.
std::expected<void*, ArenaError> Arena::alloc_zeroed(const usize bytes)
{
    const std::expected<void*, ArenaError> ptr = arena_bump(this, bytes, 1);
    if (ptr)
        memset(*ptr, 0, bytes);
    return ptr;
}

/// @brief Allocates an aligned, zeroed block of memory from the arena.
/// @param bytes The number of bytes to allocate.
/// @param alignment The alignment (must be a power of two).
/// @return The allocated pointer, or an error.
std::expected<void*, ArenaError> Arena::alloc_zeroed_aligned(const usize bytes, const usize alignment)
{
    const std::expected<void*, ArenaError> ptr = arena_bump(this, bytes, alignment);
    if (ptr)
        memset(*ptr, 0, bytes);
    return ptr;
}

/// @brief Resets the arena, decommitting pages above the commit floor.
void Arena::reset()
{
    this->used = 0;

    const usize floor = this->commit_floor;
    if (this->committed > floor)
    {
        decommit_memory(this->buffer + floor, this->committed - floor);
        this->committed = floor;
    }
}

/// @brief Shrinks the arena cursor by the given number of bytes.
/// @param bytes The number of bytes to release.
/// @return Success, or PoppedPastEnd when `bytes` exceeds the allocated amount.
ArenaError Arena::pop(const usize bytes)
{
    if (bytes > this->used)
    {
        this->used = 0;
        return ArenaError::PoppedPastEnd;
    }

    this->used -= bytes;
    return ArenaError::Success;
}
