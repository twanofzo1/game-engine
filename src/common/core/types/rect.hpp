/**
 * @file rect.hpp
 * @brief Definition of the axis-aligned rectangle type and its operations.
 */
#pragma once
#include "common/core/typedefs.hpp"
#include "common/core/types/vec2.hpp"

/// @brief Configuration used to construct a rectangle from scalars.
struct RectConfig {
    f32 x;      ///< Bottom-left X coordinate.
    f32 y;      ///< Bottom-left Y coordinate.
    f32 width;  ///< Width along the X axis.
    f32 height; ///< Height along the Y axis.
};

/// @brief Configuration used to construct a rectangle from vectors.
struct RectConfigVec2 {
    Vec2 position; ///< Bottom-left corner.
    Vec2 size;     ///< Width and height.
};

/// @brief Axis-aligned rectangle defined by a bottom-left corner and a size.
struct Rect {
    Vec2 position; ///< Bottom-left corner.
    Vec2 size;     ///< Width and height.

    static Rect create(const RectConfig config);
    static Rect create(const RectConfigVec2 config);
    Rect add(const Rect b) const;
    Rect sub(const Rect b) const;
    Rect scale(const f32 s) const;
    bool contains(const Vec2 p) const;
    bool intersects(const Rect b) const;
    Rect intersection(const Rect b) const;
};