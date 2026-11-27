/**
 * @file hurtbox.hpp
 * @brief Hurtbox type: a named region that takes damage.
 */
#pragma once
#include "common/core/types.hpp"


/** @brief A named hurtbox, as either a rect or a circle. */
struct Hurtbox {
    std::string name; ///< Unique identifier.
    Shape shape; ///< Shape of the collision.
};