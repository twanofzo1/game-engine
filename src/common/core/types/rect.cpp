/**
 * @file rect.cpp
 * @brief Implementation of the axis-aligned rectangle type operations.
 */
#include "rect.hpp"

#include <math.h>

/// @brief Constructs a rectangle from position/size scalars.
/// @param config The bottom-left coordinates and dimensions.
/// @return The initialized rectangle.
Rect Rect::create(const RectConfig config)
{
    return Rect{
        .position = Vec2::create({.x = config.x, .y = config.y}),
        .size     = Vec2::create({.x = config.width, .y = config.height}),
    };
}

/// @brief Constructs a rectangle from position and size vectors.
/// @param config The bottom-left corner and dimensions.
/// @return The initialized rectangle.
Rect Rect::create(const RectConfigVec2 config)
{
    return Rect{
        .position = config.position,
        .size     = config.size,
    };
}

/// @brief Adds a rectangle to this one component-wise (position and size).
/// @param b The rectangle to add.
/// @return The component-wise sum.
Rect Rect::add(const Rect b) const
{
    return Rect{
        .position = this->position.add(b.position),
        .size     = this->size.add(b.size),
    };
}

/// @brief Subtracts rectangle `b` from this one component-wise.
/// @param b The rectangle to subtract.
/// @return The component-wise difference.
Rect Rect::sub(const Rect b) const
{
    return Rect{
        .position = this->position.sub(b.position),
        .size     = this->size.sub(b.size),
    };
}

/// @brief Scales this rectangle's position and size by a scalar.
/// @param s The scalar multiplier.
/// @return The scaled rectangle.
Rect Rect::scale(const f32 s) const
{
    return Rect{
        .position = this->position.scale(s),
        .size     = this->size.scale(s),
    };
}

/// @brief Returns true when point `p` lies inside or on this rectangle.
/// @param p The point to test.
/// @return True when the point is inside or on the rectangle.
bool Rect::contains(const Vec2 p) const
{
    return p.x >= this->position.x && p.x <= this->position.x + this->size.x &&
           p.y >= this->position.y && p.y <= this->position.y + this->size.y;
}

/// @brief Returns true when this rectangle overlaps or touches another.
/// @param b The other rectangle.
/// @return True when the rectangles overlap or touch.
bool Rect::intersects(const Rect b) const
{
    return !(this->position.x + this->size.x < b.position.x ||
             b.position.x + b.size.x < this->position.x ||
             this->position.y + this->size.y < b.position.y ||
             b.position.y + b.size.y < this->position.y);
}

/// @brief Computes the intersection rectangle with another rectangle.
/// @param b The other rectangle.
/// @return The intersection rectangle.
Rect Rect::intersection(const Rect b) const
{
    return Rect{
        .position = Vec2::create({
            .x = fmaxf(this->position.x, b.position.x),
            .y = fmaxf(this->position.y, b.position.y),
        }),
        .size = Vec2::create({
            .x = fminf(this->position.x + this->size.x, b.position.x + b.size.x) -
                 fmaxf(this->position.x, b.position.x),
            .y = fminf(this->position.y + this->size.y, b.position.y + b.size.y) -
                 fmaxf(this->position.y, b.position.y),
        }),
    };
}
