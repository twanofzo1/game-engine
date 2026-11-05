/**
 * @file vec2.hpp
 * @brief 2D float vector type and its operations.
 */
#pragma once

#include "core/typedefs.hpp"

#include <math.h>

namespace Engine {
namespace Vec2 {

/** 2D vector with float components. */
struct Vec2 {
    f32 x; ///< X component.
    f32 y; ///< Y component.
};

/** @brief Construct a Vec2 from two components. */
static inline Vec2 make(f32 x, f32 y)
{
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

/** @brief Add two vectors component-wise. */
static inline Vec2 add(Vec2 a, Vec2 b)
{
    return make(a.x + b.x, a.y + b.y);
}

/** @brief Subtract `b` from `a` component-wise. */
static inline Vec2 sub(Vec2 a, Vec2 b)
{
    return make(a.x - b.x, a.y - b.y);
}

/** @brief Scale a vector by a scalar. */
static inline Vec2 scale(Vec2 v, f32 s)
{
    return make(v.x * s, v.y * s);
}

/** @brief Dot product of two vectors. */
static inline f32 dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

/** @brief Euclidean length of a vector. */
static inline f32 length(Vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

/**
 * @brief Return the unit vector in the same direction.
 * @warning A zero-length vector divides by zero; callers must guard.
 */
static inline Vec2 normalize(Vec2 v)
{
    f32 len = length(v);
    return scale(v, 1.0f / len);
}

/** @brief Rotate a vector counter-clockwise by `radians`. */
static inline Vec2 rotate(Vec2 v, f32 radians)
{
    f32 c = cosf(radians);
    f32 s = sinf(radians);
    return make(v.x * c - v.y * s, v.x * s + v.y * c);
}

/** @brief Linear interpolation between `a` and `b` (t in [0,1]). */
static inline Vec2 lerp(Vec2 a, Vec2 b, f32 t)
{
    return add(a, scale(sub(b, a), t));
}

} // namespace Vec2
} // namespace Engine
