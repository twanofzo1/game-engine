/**
 * @file sprite.hpp
 * @brief Sprite-sheet type: texture, frame grid, UV lookup and animation.
 *
 * A sprite is a tinted quad that displays one frame of a texture. `frames_x`
 * and `frames_y` divide the texture into a uniform grid, `frame` selects
 * which cell to show, and `uv_rect` returns that cell's texture coordinates.
 * The frame index is advanced by external code (a system, timer, etc.).
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"

namespace Engine {
namespace Sprite {

/** A textured sprite with an optional animation grid. */
struct Sprite {
    Color::Color color;     ///< Tint color (multiplies the texture).
    Vec2::Vec2   size;      ///< Width and height in world units.
    u32          texture;   ///< Texture id, or 0 for solid color.
    i32          frames_x;  ///< Horizontal frame count (>= 1).
    i32          frames_y;  ///< Vertical frame count (>= 1).
    i32          frame;     ///< Current frame, row-major (0-based).
    f32          z;         ///< Draw order: higher values draw on top.
};

/**
 * @brief Construct a white sprite showing the first frame of a grid.
 * @param texture  Texture id (0 = solid color).
 * @param size     Width and height in world units.
 * @param frames_x Horizontal frame count.
 * @param frames_y Vertical frame count.
 */
static inline Sprite make(u32 texture, Vec2::Vec2 size,
                          i32 frames_x, i32 frames_y)
{
    Sprite sprite;
    sprite.color    = Color::make(1.0f, 1.0f, 1.0f, 1.0f);
    sprite.size     = size;
    sprite.texture  = texture;
    sprite.frames_x = frames_x < 1 ? 1 : frames_x;
    sprite.frames_y = frames_y < 1 ? 1 : frames_y;
    sprite.frame    = 0;
    sprite.z        = 0.0f;
    return sprite;
}

/** @brief Total number of frames in the grid. */
static inline i32 frame_count(const Sprite* sprite)
{
    return sprite->frames_x * sprite->frames_y;
}

/**
 * @brief Write the UV rectangle [u0, v0, u1, v1] of the current frame.
 * @param sprite Sprite to sample.
 * @param out    Destination of four floats.
 */
static inline void uv_rect(const Sprite* sprite, f32* out)
{
    i32 total = frame_count(sprite);
    i32 f = sprite->frame % total;
    if (f < 0)
        f += total;

    i32 col = f % sprite->frames_x;
    i32 row = f / sprite->frames_x;

    out[0] = (f32)col / (f32)sprite->frames_x;
    out[1] = (f32)row / (f32)sprite->frames_y;
    out[2] = (f32)(col + 1) / (f32)sprite->frames_x;
    out[3] = (f32)(row + 1) / (f32)sprite->frames_y;
}

} // namespace Sprite
} // namespace Engine
