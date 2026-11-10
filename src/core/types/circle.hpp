/**
 * @file circle.hpp
 * @brief 2D circle type and its operations.
 */
#pragma once
#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

/** @brief Configuration used to construct a circle. */
struct CircleConfig {
    Vec2 position = Vec2::make({.x = 0.0f, .y = 0.0f}); ///< Center position.
    f32  radius = 0.0f;                                  ///< Radius.
};

/** @brief Circle defined by a center position and a radius. */
struct Circle {
    Vec2 position; ///< Center position.
    f32  radius;   ///< Radius.

    static Circle make(CircleConfig config);
    static Circle add(Circle a, Circle b);
    static Circle sub(Circle a, Circle b);
    static Circle scale(Circle c, f32 s);
    static bool contains(Circle c, Vec2 p);
    static bool intersects(Circle a, Circle b);
};