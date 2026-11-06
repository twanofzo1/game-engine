/**
 * @file sprite.hpp
 * @brief Sprite type: a textured quad laid out on a frame grid.
 */
#pragma once
#include "core/types.hpp"
#include "core/objects/texture.hpp"

/** @brief A sprite references a texture and a single frame of its grid. */
struct Sprite {
    std::string name;   ///< Unique identifier.
    Texture* texture;   ///< Backing texture.
    Vec2 size;          ///< Display size.
    u16 h_frames;       ///< Number of horizontal frames in the grid.
    u16 v_frames;       ///< Number of vertical frames in the grid.
    u16 frame;          ///< Currently selected frame index.
};