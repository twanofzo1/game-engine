/**
 * @file sprite.hpp
 * @brief Sprite type: a textured quad laid out on a frame grid.
 */
#pragma once
#include "common/core/types.hpp"
#include "common/objects/texture.hpp"

using SpriteID = u32;

/** @brief A sprite references a texture and a single frame of its grid. */
struct Sprite {
    SpriteID id;        ///< Unique identifier.
    std::string name;   ///< Unique identifier.
    TextureID texture;  ///< Backing texture reference.
    Vec2 size;          ///< Display size.
    u16 h_frames;       ///< Number of horizontal frames in the grid.
    u16 v_frames;       ///< Number of vertical frames in the grid.
    u16 current_frame;         ///< Current frame index in [0, h_frames*v_frames).
};