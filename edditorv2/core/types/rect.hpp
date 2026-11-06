/**
 * @file rect.hpp
 * @brief Definition of the axis-aligned rectangle type and its operations.
 */
#pragma once
#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

/** @brief Configuration used to construct a rectangle from scalars. */
struct RectConfig {
    f32 x;      ///< Bottom-left X coordinate.
    f32 y;      ///< Bottom-left Y coordinate.
    f32 width;  ///< Width along the X axis.
    f32 height; ///< Height along the Y axis.
};

/** @brief Configuration used to construct a rectangle from vectors. */
struct RectConfigVec2 {
    Vec2 position; ///< Bottom-left corner.
    Vec2 size;     ///< Width and height.
};

/** @brief Axis-aligned rectangle defined by a bottom-left corner and a size. */
struct Rect {
    Vec2 position; ///< Bottom-left corner.
    Vec2 size;     ///< Width and height.

    static Rect make(RectConfig config);
    static Rect make(RectConfigVec2 config);
    static Rect add(Rect a, Rect b);
    static Rect sub(Rect a, Rect b);
    static Rect scale(Rect r, f32 s);
    static bool contains(Rect r, Vec2 p);
    static bool intersects(Rect a, Rect b);
    static Rect intersection(Rect a, Rect b);
};