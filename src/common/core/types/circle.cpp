/**
 * @file circle.cpp
 * @brief Implementation of the circle type operations.
 */
#include "circle.hpp"

/// @brief Constructs a circle from its position and radius.
/// @param config The center position and radius.
/// @return The initialized circle.
Circle Circle::create(const CircleConfig config)
{
    return Circle{
        .position = config.position,
        .radius   = config.radius,
    };
}

/// @brief Adds a circle to this one component-wise (position and radius).
/// @param b The circle to add.
/// @return The component-wise sum.
Circle Circle::add(const Circle b) const
{
    return Circle{
        .position = this->position.add(b.position),
        .radius   = this->radius + b.radius,
    };
}

/// @brief Subtracts circle `b` from this one component-wise.
/// @param b The circle to subtract.
/// @return The component-wise difference.
Circle Circle::sub(const Circle b) const
{
    return Circle{
        .position = this->position.sub(b.position),
        .radius   = this->radius - b.radius,
    };
}

/// @brief Scales this circle's position and radius by a scalar.
/// @param s The scalar multiplier.
/// @return The scaled circle.
Circle Circle::scale(const f32 s) const
{
    return Circle{
        .position = this->position.scale(s),
        .radius   = this->radius * s,
    };
}

/// @brief Returns true when point `p` lies inside or on this circle.
/// @param p The point to test.
/// @return True when the point is inside or on the circle.
bool Circle::contains(const Vec2 p) const
{
    return this->position.distance(p) <= this->radius;
}

/// @brief Returns true when this circle overlaps or touches another.
/// @param b The other circle.
/// @return True when the circles overlap or touch.
bool Circle::intersects(const Circle b) const
{
    return this->position.distance(b.position) <= this->radius + b.radius;
}
