/**
 * @file recti.hpp
 * @brief Axis-aligned integer rectangle type and its operations.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2i.hpp"

namespace Engine {
namespace Recti {

/** Axis-aligned rectangle defined by a bottom-left corner and a size. */
struct Recti {
    Vec2i::Vec2i position; ///< Bottom-left corner in world units.
    Vec2i::Vec2i size;     ///< Width and height in world units.
};

/** @brief Construct a Recti from corner and size. */
static inline Recti make(i32 x, i32 y, i32 width, i32 height)
{
    Recti r;
    r.position = Vec2i::make(x, y);
    r.size     = Vec2i::make(width, height);
    return r;
}

/**
 * @brief Center point of the rectangle.
 * @note Uses integer division, so odd widths/heights round toward zero.
 */
static inline Vec2i::Vec2i center(Recti r)
{
    return Vec2i::make(r.position.x + r.size.x / 2,
                       r.position.y + r.size.y / 2);
}

/** @brief Left edge (minimum X). */
static inline i32 left(Recti r) { return r.position.x; }

/** @brief Right edge (maximum X). */
static inline i32 right(Recti r) { return r.position.x + r.size.x; }

/** @brief Bottom edge (minimum Y). */
static inline i32 bottom(Recti r) { return r.position.y; }

/** @brief Top edge (maximum Y). */
static inline i32 top(Recti r) { return r.position.y + r.size.y; }

/** @brief Return true when `point` is inside the rectangle (edges inclusive). */
static inline bool contains_point(Recti r, Vec2i::Vec2i point)
{
    return point.x >= left(r) && point.x <= right(r)
        && point.y >= bottom(r) && point.y <= top(r);
}

/** @brief Return true when two rectangles overlap (touching edges do not count). */
static inline bool intersects(Recti a, Recti b)
{
    return left(a) < right(b) && right(a) > left(b)
        && bottom(a) < top(b) && top(a) > bottom(b);
}

} // namespace Recti
} // namespace Engine
