/**
 * @file collision.hpp
 * @brief Collision type: a named shape used for collision detection.
 */
#pragma once
#include "common/core/types.hpp"



/** @brief A named collision shape, as either a rect or a circle. */
struct Collision {
    std::string name; ///< Unique identifier.
    Shape shape; ///< Shape of the collision.
};