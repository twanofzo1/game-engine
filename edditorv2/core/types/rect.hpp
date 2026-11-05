/**
 * @file rect.hpp
 * @brief Definition of the axis-aligned rectangle type and its operations.
*/

#pragma once
#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

/** Configuration used to construct a rectangle from scalars. */
struct Rect2Config {
    float x;      ///< Bottom-left X coordinate.
    float y;      ///< Bottom-left Y coordinate.
    float width;  ///< Width along the X axis.
    float height; ///< Height along the Y axis.
};

/** Configuration used to construct a rectangle from vectors. */
struct Rect2ConfigVec2 {
    Vec2 position; ///< Bottom-left corner.
    Vec2 size;     ///< Width and height.
};

/** Axis-aligned rectangle defined by a bottom-left corner and a size. */
struct Rect2 {
    Vec2 position; ///< Bottom-left corner.
    Vec2 size;     ///< Width and height.

    static Rect2 make(Rect2Config config);
    static Rect2 make(Rect2ConfigVec2 config);
    static Rect2 add(Rect2 a, Rect2 b);
    static Rect2 sub(Rect2 a, Rect2 b);
    static Rect2 scale(Rect2 r, float s);
    static bool contains(Rect2 r, Vec2 p);
    static bool intersects(Rect2 a, Rect2 b);
    static Rect2 intersection(Rect2 a, Rect2 b);
};