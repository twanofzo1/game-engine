/**
 * @file hitbox.hpp
 * @brief Hitbox type: a named region that deals damage.
 */
#pragma once
#include "core/types.hpp"

/** @brief A named hitbox, as either a rect or a circle. */
struct Hitbox {
    std::string name; ///< Unique identifier.
    union {
        struct {
            Rect rect; ///< Rectangular shape.
        };
        struct {
            Circle circle; ///< Circular shape.
        };
    };
};
