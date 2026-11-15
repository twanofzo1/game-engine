/**
 * @file stamina.hpp
 * @brief Stamina type: current, maximum, and regeneration rate.
 */
#pragma once
#include "common/core/types.hpp"

/** @brief Tracks current and maximum stamina plus a regeneration rate. */
struct Stamina {
    u16 max;        ///< Maximum stamina.
    u16 regen_rate; ///< Stamina regained per unit of time.
};