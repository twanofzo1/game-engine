/**
 * @file hurtbox.hpp
 * @brief Hurtbox type: a named region that takes damage.
 */
#pragma once
#include "core/types.hpp"

/** @brief A named hurtbox, as either a rect or a circle. */
struct Hurtbox {
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