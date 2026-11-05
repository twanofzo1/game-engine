/**
 * @file vec2i.hpp
 * @brief 2D integer vector type and its operations.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Vec2i {

/** 2D vector with integer components. */
struct Vec2i {
    i32 x; ///< X component.
    i32 y; ///< Y component.
};

/** @brief Construct a Vec2i from two components. */
static inline Vec2i make(i32 x, i32 y)
{
    Vec2i v;
    v.x = x;
    v.y = y;
    return v;
}

/** @brief Add two vectors component-wise. */
static inline Vec2i add(Vec2i a, Vec2i b)
{
    return make(a.x + b.x, a.y + b.y);
}

/** @brief Subtract `b` from `a` component-wise. */
static inline Vec2i sub(Vec2i a, Vec2i b)
{
    return make(a.x - b.x, a.y - b.y);
}

/** @brief Scale a vector by an integer scalar. */
static inline Vec2i scale(Vec2i v, i32 s)
{
    return make(v.x * s, v.y * s);
}

/** @brief Return true when both components are equal. */
static inline bool equal(Vec2i a, Vec2i b)
{
    return a.x == b.x && a.y == b.y;
}

/** @brief Dot product of two vectors (64-bit to avoid overflow). */
static inline i64 dot(Vec2i a, Vec2i b)
{
    return (i64)a.x * b.x + (i64)a.y * b.y;
}

} // namespace Vec2i
} // namespace Engine
