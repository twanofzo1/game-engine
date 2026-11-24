/**
 * @file circle.hpp
 * @brief 2D circle type and its operations.
 */
#pragma once
#include "common/core/typedefs.hpp"
#include "common/core/types/vec2.hpp"

/// @brief Configuration used to construct a circle.
struct CircleConfig {
    Vec2 position = Vec2::create({.x = 0.0f, .y = 0.0f}); ///< Center position.
    f32  radius = 0.0f;                                    ///< Radius.
};

/// @brief Circle defined by a center position and a radius.
struct Circle {
    Vec2 position; ///< Center position.
    f32  radius;   ///< Radius.

    static Circle create(const CircleConfig config);
    Circle add(const Circle b) const;
    Circle sub(const Circle b) const;
    Circle scale(const f32 s) const;
    bool contains(const Vec2 p) const;
    bool intersects(const Circle b) const;
};