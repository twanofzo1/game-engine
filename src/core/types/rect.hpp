/**
 * @file rect.hpp
 * @brief Axis-aligned float rectangle type and its operations.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

namespace Engine {
namespace Rect {

/** Axis-aligned rectangle defined by a bottom-left corner and a size. */
struct Rect {
    Vec2::Vec2 position; ///< Bottom-left corner in world units.
    Vec2::Vec2 size;     ///< Width and height in world units.
};

/** @brief Construct a Rect from corner and size. */
static inline Rect make(f32 x, f32 y, f32 width, f32 height)
{
    Rect r;
    r.position = Vec2::make(x, y);
    r.size     = Vec2::make(width, height);
    return r;
}

/** @brief Center point of the rectangle. */
static inline Vec2::Vec2 center(Rect r)
{
    return Vec2::add(r.position, Vec2::scale(r.size, 0.5f));
}

/** @brief Left edge (minimum X). */
static inline f32 left(Rect r) { return r.position.x; }

/** @brief Right edge (maximum X). */
static inline f32 right(Rect r) { return r.position.x + r.size.x; }

/** @brief Bottom edge (minimum Y). */
static inline f32 bottom(Rect r) { return r.position.y; }

/** @brief Top edge (maximum Y). */
static inline f32 top(Rect r) { return r.position.y + r.size.y; }

/** @brief Return true when `point` is inside the rectangle (edges inclusive). */
static inline bool contains_point(Rect r, Vec2::Vec2 point)
{
    return point.x >= left(r) && point.x <= right(r)
        && point.y >= bottom(r) && point.y <= top(r);
}

/** @brief Return true when two rectangles overlap (touching edges do not count). */
static inline bool intersects(Rect a, Rect b)
{
    return left(a) < right(b) && right(a) > left(b)
        && bottom(a) < top(b) && top(a) > bottom(b);
}

} // namespace Rect
} // namespace Engine
