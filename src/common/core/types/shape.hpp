/**
 * @file shape.hpp
 * @brief Shape type: a geometric figure used for collision detection.
 */
#pragma once
#include "common/core/typedefs.hpp"
#include "common/core/types/circle.hpp"
#include "common/core/types/rect.hpp"

/** @brief A geometric figure: either an axis-aligned rectangle or a circle. */
struct Shape {
    /// @brief The discriminator identifying the active variant.
    enum class Type {
        Rect,   ///< The shape holds a rectangle.
        Circle  ///< The shape holds a circle.
    };

    Type type; ///< Type of the shape (which union member is active).

    /** @brief Untagged storage for the actual geometry. */
    union {
        struct {
            Rect rect; ///< Rectangular shape data (valid when type is Rect).
        };
        struct {
            Circle circle; ///< Circular shape data (valid when type is Circle).
        };
    };

    /** @brief Construct a rectangular shape. */
    static Shape make_rect(const Rect rect);

    /** @brief Construct a circular shape. */
    static Shape make_circle(const Circle circle);
};