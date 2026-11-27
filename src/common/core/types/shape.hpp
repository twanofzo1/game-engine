/**
 * @file shape.hpp
 * @brief Shape type: a geometric figure used for collision detection.
 */
#pragma once
#include "common/core/typedefs.hpp"
#include "common/core/types/circle.hpp"
#include "common/core/types/rect.hpp"


enum class ShapeType{
    Rect,   ///< The shape holds a rectangle.
    Circle  ///< The shape holds a circle.
};
/// @brief A geometric figure: either an axis-aligned rectangle or a circle.
struct Shape {
    /// @brief The discriminator identifying the active variant.
    ShapeType type; ///< Type of the shape (which union member is active).

    /// @brief Untagged storage for the actual geometry.
    union {
        Rect rect;     ///< Axis-aligned rectangle.
        Circle circle; ///< Circle.
    };

    static Shape create(const Rect rect);
    static Shape create(const Circle circle);
};