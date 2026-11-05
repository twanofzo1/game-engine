/**
 * @file typedefs.hpp
 * @brief Global fixed-width primitive type aliases.
 *
 * These aliases are intentionally left in the global namespace (like the
 * C <stdint.h> types they wrap) because every module uses them; namespacing
 * them would only add noise. Engine-specific types live in the Engine::*
 * namespaces.
 */
#pragma once

#include <cstdint>

typedef std::uint8_t  u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;
typedef std::int8_t   i8;
typedef std::int16_t  i16;
typedef std::int32_t  i32;
typedef std::int64_t  i64;
typedef float  f32;
typedef double f64;
typedef std::size_t    usize;
typedef std::ptrdiff_t isize;
typedef std::uint8_t   byte;

/** Subscriber callback: invoked with an event payload (void*). */
typedef void (*Subscriber)(void* payload);