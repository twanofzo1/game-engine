/**
 * @file animation_player.hpp
 * @brief Animation player type: plays a set of animations over time.
 */
#pragma once
#include "core/typedefs.hpp"
#include "animation.hpp"

/** @brief Plays back a collection of animations with a playback speed. */
struct AnimationPlayer {
    std::string name;                       ///< Unique identifier.
    std::vector<Animation> animations;      ///< Animations available to play.
    f32 speed;                              ///< Playback speed multiplier.
};