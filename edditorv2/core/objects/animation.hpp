/**
 * @file animation.hpp
 * @brief Animation type: a named set of sprite frames.
 */
#pragma once
#include "sprite.hpp"
#include "core/typedefs.hpp"
#include "core/types.hpp"

/** @brief A named sequence of sprite frames used to drive a sprite visually. */
struct Animation {
    std::string name;                                  ///< Unique identifier.
    Sprite* sprite;                                    ///< Backing sprite sheet.
    std::vector<std::pair<f32, u16>> frames;           ///< Pairs of (timestamp, frame index).
};
