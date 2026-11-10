/**
 * @file health.hpp
 * @brief Health type: current and maximum hit points.
 */
#pragma once
#include "core/types.hpp"   

#include "core/objects/signal.hpp"

/** @brief Tracks current and maximum hit points. */
struct Health {
    u16 initial;      ///< Initial hit points.
    u16 max;          ///< Maximum hit points.
    Signal on_death;  ///< Signal to emit when health reaches zero.
    Signal on_damage; ///< Signal to emit when health is reduced.
    Signal on_heal;   ///< Signal to emit when health is increased.
};