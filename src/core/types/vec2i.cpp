/**
 * @file vec2i.cpp
 * @brief Implementation of the 2D integer vector type operations.
 */

#include "vec2i.hpp"


/** @brief Construct a Vec2i from a configuration. */
Vec2i Vec2i::make(Vec2iConfig config)
{
    return Vec2i{
        .x = config.x,
        .y = config.y
    };
}

/** @brief Add two vectors component-wise. */
Vec2i Vec2i::add(Vec2i a, Vec2i b)
{
    return Vec2i::make({.x = a.x + b.x, .y = a.y + b.y});
}

/** @brief Subtract `b` from `a` component-wise. */
Vec2i Vec2i::sub(Vec2i a, Vec2i b)
{
    return Vec2i::make({.x = a.x - b.x, .y = a.y - b.y});
}

/** @brief Scale a vector by an integer scalar. */
Vec2i Vec2i::scale(Vec2i v, i32 s)
{
    return Vec2i::make({.x = v.x * s, .y = v.y * s});
}

/** @brief Return true when both components are equal. */
bool Vec2i::equal(Vec2i a, Vec2i b)
{
    return a.x == b.x && a.y == b.y;
}

/** @brief Dot product of two vectors (64-bit to avoid overflow). */
i64 Vec2i::dot(Vec2i a, Vec2i b)
{
    return (i64)a.x * b.x + (i64)a.y * b.y;
}