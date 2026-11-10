/**
 * @file hurtbox.hpp
 * @brief Hurtbox type: a named region that takes damage.
 */
#pragma once
#include "core/types.hpp"

enum class HurtboxShape {
    Rect,
    Circle
};

/** @brief A named hurtbox, as either a rect or a circle. */
struct Hurtbox {
    std::string name; ///< Unique identifier.
    HurtboxShape shape; ///< Shape of the hurtbox.
    union {
        struct {
            Rect rect; ///< Rectangular shape.
        };
        struct {
            Circle circle; ///< Circular shape.
        };
    };
};