/**
 * @file rect.cpp
 * @brief Implementation of the axis-aligned rectangle type operations.
 */
#include "rect.hpp"

#include <math.h>


/** @brief Construct a rectangle from position/size scalars. */
Rect Rect::make(RectConfig config)
{
    return Rect{
        .position = Vec2::make({.x = config.x, .y = config.y}),
        .size     = Vec2::make({.x = config.width, .y = config.height})
    };
}

/** @brief Construct a rectangle from position and size vectors. */
Rect Rect::make(RectConfigVec2 config)
{
    return Rect{
        .position = config.position,
        .size     = config.size
    };
}

/** @brief Add two rectangles component-wise (position and size). */
Rect Rect::add(Rect a, Rect b)
{
    return Rect{
        .position = Vec2::add(a.position, b.position),
        .size     = Vec2::add(a.size, b.size)
    };
}

/** @brief Subtract rectangle `b` from `a` component-wise. */
Rect Rect::sub(Rect a, Rect b)
{
    return Rect{
        .position = Vec2::sub(a.position, b.position),
        .size     = Vec2::sub(a.size, b.size)
    };
}

/** @brief Scale a rectangle's position and size by a scalar. */
Rect Rect::scale(Rect r, f32 s)
{
    return Rect{
        .position = Vec2::scale(r.position, s),
        .size     = Vec2::scale(r.size, s)
    };
}

/** @brief Return true when point `p` lies inside or on rectangle `r`. */
bool Rect::contains(Rect r, Vec2 p)
{
    return p.x >= r.position.x && p.x <= r.position.x + r.size.x &&
           p.y >= r.position.y && p.y <= r.position.y + r.size.y;
}

/** @brief Return true when two rectangles overlap or touch. */
bool Rect::intersects(Rect a, Rect b)
{
    return !(a.position.x + a.size.x < b.position.x ||
             b.position.x + b.size.x < a.position.x ||
             a.position.y + a.size.y < b.position.y ||
             b.position.y + b.size.y < a.position.y);
}

/** @brief Return the intersection rectangle of two overlapping rectangles. */
Rect Rect::intersection(Rect a, Rect b)
{
    return Rect{
        .position = Vec2::make({
            .x = fmaxf(a.position.x, b.position.x),
            .y = fmaxf(a.position.y, b.position.y)
        }),
        .size = Vec2::make({
            .x = fminf(a.position.x + a.size.x, b.position.x + b.size.x) -
                 fmaxf(a.position.x, b.position.x),
            .y = fminf(a.position.y + a.size.y, b.position.y + b.size.y) -
                 fmaxf(a.position.y, b.position.y)
        })
    };
}
