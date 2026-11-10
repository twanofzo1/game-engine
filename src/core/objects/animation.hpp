/**
 * @file animation.hpp
 * @brief Animation type: a named set of sprite frames.
 */
#pragma once
#include "sprite.hpp"
#include "core/typedefs.hpp"
#include "core/types.hpp"


using AnimationID = u32; ///< Unique identifier for an animation.
/** @brief A named sequence of sprite frames used to drive a sprite visually. */
struct Animation {
    AnimationID id;                                    ///< Unique identifier.
    std::string name;                                  ///< Unique identifier.
    std::vector<std::pair<f32, u16>> frames;           ///< Pairs of (timestamp, frame index).
};
