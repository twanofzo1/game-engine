#include "vec2i.hpp"

/** @brief Construct a Vec2i from a configuration. */
static inline Vec2i make(Vec2iConfig config)
{
    Vec2i v;
    v.x = config.x;
    v.y = config.y;
    return v;
}

/** @brief Add two vectors component-wise. */
static inline Vec2i add(Vec2i a, Vec2i b)
{
    return make({
        .x = a.x + b.x, 
        .y = a.y + b.y
    });
}

/** @brief Subtract `b` from `a` component-wise. */
static inline Vec2i sub(Vec2i a, Vec2i b)
{
    return make({
        .x = a.x - b.x, 
        .y = a.y - b.y
    });
}

/** @brief Scale a vector by an integer scalar. */
static inline Vec2i scale(Vec2i v, i32 s)
{
    return make({
        .x = v.x * s, 
        .y = v.y * s
    });
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

