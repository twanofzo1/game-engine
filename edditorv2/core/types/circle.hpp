#pragma once
#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

/** Configuration used to construct a circle. */
struct CircleConfig {
    Vec2 position = Vec2::make({.x = 0.0f, .y = 0.0f}); ///< Center position.
    float radius = 0.0f ;                               ///< Radius.
};

/** Circle defined by a center position and a radius. */
struct Circle {
    Vec2 position; ///< Center position.
    float radius;  ///< Radius.

    static Circle make(CircleConfig config);
    static Circle add(Circle a, Circle b);
    static Circle sub(Circle a, Circle b);
    static Circle scale(Circle c, float s);
    static bool contains(Circle c, Vec2 p);
    static bool intersects(Circle a, Circle b);
};