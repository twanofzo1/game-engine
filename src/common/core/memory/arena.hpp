/**
 * @file arena.hpp
 * @brief Declares the virtual-memory-backed linear (bump) allocator.
 *
 * The arena reserves a large virtual address range up front and commits
 * physical pages on demand as allocations grow, so it can cheaply hold many
 * short-lived allocations with no per-block overhead.
 */
#pragma once

#include "common/core/types.hpp"
#include <expected>


#define KB(x) ((x) * 1024)
#define MB(x) (KB(x) * 1024)
#define GB(x) (MB(x) * 1024)

/** Options used when creating an arena. */
struct ArenaConfig {
    usize reserved = 0; ///< Total address space to reserve, in bytes.
    usize commit   = 0; ///< Bytes to commit up front (must be <= reserved).
};

/** Error codes returned by fallible arena operations. */
enum class ArenaError : u8 {
    Success          = 0, ///< No error.
    ReserveFailed    = 1, ///< The OS could not reserve the address space.
    CommitFailed     = 2, ///< The OS could not commit virtual memory pages.
    OutOfReserved    = 3, ///< An allocation exceeds the reserved address space.
    InvalidConfig    = 4, ///< The given config is invalid (e.g. commit > reserved).
    InvalidAlignment = 5, ///< The given alignment is not a power of two.
    PoppedPastEnd    = 6, ///< A pop() call would free more memory than allocated.
};

/** A linear allocator backed by a reserved/committed virtual memory range. */
struct Arena {
    u8*    buffer;       ///< Base address of the reserved memory range.
    usize  reserved;     ///< Total reserved address space, in bytes.
    usize  committed;    ///< Currently committed bytes.
    usize  used;         ///< Bytes currently allocated from the arena.
    usize  commit_floor; ///< Minimum committed bytes kept by reset().

    static std::expected<Arena, ArenaError> make(const ArenaConfig config);

    void destroy();
    std::expected<void*, ArenaError> alloc(usize bytes);
    std::expected<void*, ArenaError> alloc_aligned(usize bytes, usize alignment);
    std::expected<void*, ArenaError> alloc_zeroed(usize bytes);
    std::expected<void*, ArenaError> alloc_zeroed_aligned(usize bytes, usize alignment);
    void reset();
    ArenaError pop(usize bytes);
};


