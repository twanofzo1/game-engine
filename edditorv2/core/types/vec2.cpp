/**
 * @file vec2.hpp
 * @brief 2D float vector type and its operations.
 */

#include "vec2.hpp"

/** @brief Create a new vector from a configuration. */
static Vec2 make(Vec2Config config)
{
    Vec2 v;
    v.x = config.x;
    v.y = config.y;
    return v;
}

/** @brief Add two vectors component-wise. */
static inline Vec2 add(Vec2 a, Vec2 b)
{
    return make({
        .x = a.x + b.x, 
        .y = a.y + b.y
    });
}

/** @brief Subtract `b` from `a` component-wise. */
static inline Vec2 sub(Vec2 a, Vec2 b)
{
    return make({
        .x = a.x - b.x, 
        .y = a.y - b.y
    });
}

/** @brief Scale a vector by a scalar. */
static inline Vec2 scale(Vec2 v, f32 s)
{
    return make({
        .x = v.x * s, 
        .y = v.y * s
    });
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
    return make({
        .x = v.x * c - v.y * s,
        .y = v.x * s + v.y * c
    });
}

/** @brief Linear interpolation between `a` and `b` (t in [0,1]). */
static inline Vec2 lerp(Vec2 a, Vec2 b, f32 t)
{
    return add(a, scale(sub(b, a), t));
}

static inline f32 distance(Vec2 a, Vec2 b)
{
    return length(sub(a, b));
}