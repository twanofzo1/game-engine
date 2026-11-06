/**
 * @file circle.cpp
 * @brief Implementation of the circle type operations.
 */
#include "circle.hpp"

/** @brief Construct a circle from a configuration. */
Circle Circle::make(CircleConfig config)
{
    return Circle{
        .position = config.position,
        .radius   = config.radius
    };
}

/** @brief Add two circles component-wise (position and radius). */
Circle Circle::add(Circle a, Circle b)
{
    return Circle{
        .position = Vec2::add(a.position, b.position),
        .radius   = a.radius + b.radius
    };
}

/** @brief Subtract circle `b` from `a` component-wise. */
Circle Circle::sub(Circle a, Circle b)
{
    return Circle{
        .position = Vec2::sub(a.position, b.position),
        .radius   = a.radius - b.radius
    };
}

/** @brief Scale a circle's position and radius by a scalar. */
Circle Circle::scale(Circle c, f32 s)
{
    return Circle{
        .position = Vec2::scale(c.position, s),
        .radius   = c.radius * s
    };
}

/** @brief Return true when point `p` lies inside or on circle `c`. */
bool Circle::contains(Circle c, Vec2 p)
{
    return Vec2::distance(c.position, p) <= c.radius;
}

/** @brief Return true when two circles overlap or touch. */
bool Circle::intersects(Circle a, Circle b)
{
    return Vec2::distance(a.position, b.position) <= a.radius + b.radius;
}
