/**
 * @file health.hpp
 * @brief Health type: current and maximum hit points.
 */
#pragma once
#include "core/typedefs.hpp"

/** @brief Tracks current and maximum hit points. */
struct Health {
    u16 current; ///< Current hit points.
    u16 max;     ///< Maximum hit points.
};