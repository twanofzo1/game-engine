/**
 * @file collision.hpp
 * @brief Collision type: a named shape used for collision detection.
 */
#pragma once
#include "common/core/types.hpp"

enum class CollisionShape {
    Rect,
    Circle
};

/** @brief A named collision shape, as either a rect or a circle. */
struct Collision {
    std::string name; ///< Unique identifier.
    CollisionShape shape; ///< Shape of the collision.
    union {
        struct {
            Rect rect; ///< Rectangular shape.
        };
        struct {
            Circle circle; ///< Circular shape.
        };
    };
};