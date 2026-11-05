/**
 * @file rect.cpp
 * @brief Implementation of the axis-aligned rectangle type operations.
 */
#include "rect.hpp"

/** @brief Construct a rectangle from position/size scalars. */
static Rect2 make(Rect2Config config){
    return Rect2{
        .position = Vec2::make({.x = config.x, .y = config.y}),
        .size     = Vec2::make({.x = config.width, .y = config.height})
    };
}

/** @brief Construct a rectangle from position and size vectors. */
static Rect2 make(Rect2ConfigVec2 config){
    return Rect2{
        .position = config.position,
        .size     = config.size
    };
}

/** @brief Add two rectangles component-wise (position and size). */
static Rect2 add(Rect2 a, Rect2 b){
    return Rect2{
        .position = Vec2::add(a.position, b.position),
        .size     = Vec2::add(a.size, b.size)
    };
}

/** @brief Subtract rectangle `b` from `a` component-wise. */
static Rect2 sub(Rect2 a, Rect2 b){
    return Rect2{
        .position = Vec2::sub(a.position, b.position),
        .size     = Vec2::sub(a.size, b.size)
    };
}

/** @brief Scale a rectangle's position and size by a scalar. */
static Rect2 scale(Rect2 r, float s){
    return Rect2{
        .position = Vec2::scale(r.position, s),
        .size     = Vec2::scale(r.size, s)
    };
}

/** @brief Return true when point `p` lies inside or on rectangle `r`. */
static bool contains(Rect2 r, Vec2 p){
    return p.x >= r.position.x && p.x <= r.position.x + r.size.x &&
           p.y >= r.position.y && p.y <= r.position.y + r.size.y;
}

/** @brief Return true when two rectangles overlap or touch. */
static bool intersects(Rect2 a, Rect2 b){
    return !(a.position.x + a.size.x < b.position.x ||
             b.position.x + b.size.x < a.position.x ||
             a.position.y + a.size.y < b.position.y ||
             b.position.y + b.size.y < a.position.y);
}

/** @brief Return the intersection rectangle of two overlapping rectangles. */
static Rect2 intersection(Rect2 a, Rect2 b){
    return Rect2{
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
