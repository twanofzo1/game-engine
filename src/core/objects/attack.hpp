/**
 * @file attack.hpp
 * @brief Attack type.
 */
#pragma once
#include "core/types.hpp"

#include "core/objects/animation.hpp"
#include "core/objects/hitbox.hpp"
#include "core/objects/signal.hpp"

/** @brief An attack definition. */
struct Attack {
    AnimationID animation;                     ///< Animation to play when the attack is executed.
    u32 damage;                                ///< Damage to deal when the attack is executed.
    u32 knockback_multiplier;                  ///< Knockback to apply when the attack is executed.
    std::vector<Hitbox> hitboxes;              ///< Hitboxes to use when the attack is executed.
    std::vector<Vec2> knockback_directions;    ///< Knockback directions to apply when the attack is executed.
    std::vector<Signal> signals;               ///< Signals to emit when the attack is executed.
};