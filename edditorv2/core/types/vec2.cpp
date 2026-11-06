/**
 * @file vec2.cpp
 * @brief Implementation of the 2D float vector type operations.
 */

#include "vec2.hpp"

#include <math.h>


/** @brief Create a new vector from a configuration. */
Vec2 Vec2::make(Vec2Config config)
{
    return Vec2{
        .x = config.x,
        .y = config.y
    };
}

/** @brief Add two vectors component-wise. */
Vec2 Vec2::add(Vec2 a, Vec2 b)
{
    return Vec2::make({.x = a.x + b.x, .y = a.y + b.y});
}

/** @brief Subtract `b` from `a` component-wise. */
Vec2 Vec2::sub(Vec2 a, Vec2 b)
{
    return Vec2::make({.x = a.x - b.x, .y = a.y - b.y});
}

/** @brief Scale a vector by a scalar. */
Vec2 Vec2::scale(Vec2 v, f32 s)
{
    return Vec2::make({.x = v.x * s, .y = v.y * s});
}

/** @brief Dot product of two vectors. */
f32 Vec2::dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

/** @brief Euclidean length of a vector. */
f32 Vec2::length(Vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

/**
 * @brief Return the unit vector in the same direction.
 * @warning A zero-length vector divides by zero; callers must guard.
 */
Vec2 Vec2::normalize(Vec2 v)
{
    f32 len = Vec2::length(v);
    return Vec2::scale(v, 1.0f / len);
}

/** @brief Rotate a vector counter-clockwise by `radians`. */
Vec2 Vec2::rotate(Vec2 v, f32 radians)
{
    f32 c = cosf(radians);
    f32 s = sinf(radians);
    return Vec2::make({.x = v.x * c - v.y * s, .y = v.x * s + v.y * c});
}

/** @brief Linear interpolation between `a` and `b` (t in [0,1]). */
Vec2 Vec2::lerp(Vec2 a, Vec2 b, f32 t)
{
    return Vec2::add(a, Vec2::scale(Vec2::sub(b, a), t));
}

/** @brief Euclidean distance between two vectors. */
f32 Vec2::distance(Vec2 a, Vec2 b)
{
    return Vec2::length(Vec2::sub(a, b));
}