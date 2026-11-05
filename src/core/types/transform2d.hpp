/**
 * @file transform2d.hpp
 * @brief 2D float transform (position, rotation, scale) and its operations.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

namespace Engine {
namespace Transform2D {

/** 2D transform made of a position, a rotation and a 2D scale. */
struct Transform2D {
    Vec2::Vec2 position; ///< 2D position.
    f32        rotation; ///< Rotation in radians.
    Vec2::Vec2 scale;    ///< 2D scale factors.
};

/** @brief Construct a Transform2D. */
static inline Transform2D make(Vec2::Vec2 position, f32 rotation, Vec2::Vec2 scale)
{
    Transform2D t;
    t.position = position;
    t.rotation = rotation;
    t.scale    = scale;
    return t;
}

/**
 * @brief Transform a local-space point to world space.
 * Applies scale, then rotation, then translation.
 */
static inline Vec2::Vec2 apply(Transform2D t, Vec2::Vec2 point)
{
    Vec2::Vec2 scaled  = Vec2::make(point.x * t.scale.x, point.y * t.scale.y);
    Vec2::Vec2 rotated = Vec2::rotate(scaled, t.rotation);
    return Vec2::add(rotated, t.position);
}

/**
 * @brief Compose a parent and a local transform into a world transform.
 * The result maps a leaf point through `local` then through `parent`.
 */
static inline Transform2D compose(Transform2D parent, Transform2D local)
{
    Transform2D out;
    out.position = Vec2::add(parent.position,
                             Vec2::rotate(Vec2::make(local.position.x * parent.scale.x,
                                                     local.position.y * parent.scale.y),
                                          parent.rotation));
    out.rotation = parent.rotation + local.rotation;
    out.scale    = Vec2::make(parent.scale.x * local.scale.x,
                              parent.scale.y * local.scale.y);
    return out;
}

} // namespace Transform2D
} // namespace Engine
