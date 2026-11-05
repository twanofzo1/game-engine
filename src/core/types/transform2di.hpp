/**
 * @file transform2di.hpp
 * @brief 2D integer transform and its operations.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2i.hpp"

#include <math.h>

namespace Engine {
namespace Transform2Di {

/** 2D transform with integer position/scale and a float rotation. */
struct Transform2Di {
    Vec2i::Vec2i position; ///< 2D position.
    f32          rotation; ///< Rotation in radians.
    Vec2i::Vec2i scale;    ///< 2D scale factors.
};

/** @brief Construct a Transform2Di. */
static inline Transform2Di make(Vec2i::Vec2i position, f32 rotation, Vec2i::Vec2i scale)
{
    Transform2Di t;
    t.position = position;
    t.rotation = rotation;
    t.scale    = scale;
    return t;
}

/**
 * @brief Transform a local-space point to world space.
 * Applies scale, then rotation, then translation, rounding to integers.
 */
static inline Vec2i::Vec2i apply(Transform2Di t, Vec2i::Vec2i point)
{
    f32 sx = (f32)point.x * (f32)t.scale.x;
    f32 sy = (f32)point.y * (f32)t.scale.y;
    f32 c  = cosf(t.rotation);
    f32 s  = sinf(t.rotation);

    f32 rx = sx * c - sy * s;
    f32 ry = sx * s + sy * c;

    return Vec2i::make(t.position.x + (i32)lroundf(rx),
                       t.position.y + (i32)lroundf(ry));
}

} // namespace Transform2Di
} // namespace Engine
