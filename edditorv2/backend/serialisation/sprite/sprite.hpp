#pragma once
#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"

struct SpriteConfig {
    Color color;     ///< Tint color (multiplies the texture).
    Vec2  size;      ///< Width and height in world units.
    u32   texture;   ///< Texture id, or 0 for solid color.
    i32   frames_x;  ///< Horizontal frame count (>= 1).
    i32   frames_y;  ///< Vertical frame count (>= 1).
    i32   frame;     ///< Current frame, row-major (0-based).
    f32   z;         ///< Draw order: higher values draw on top.
};

struct Sprite {
    Color        color;     ///< Tint color (multiplies the texture).
    Vec2         size;      ///< Width and height in world units.
    u32          texture;   ///< Texture id, or 0 for solid color.
    i32          frames_x;  ///< Horizontal frame count (>= 1).
    i32          frames_y;  ///< Vertical frame count (>= 1).
    i32          frame;     ///< Current frame, row-major (0-based).
    f32          z;         ///< Draw order: higher values draw on top.

    static Sprite make(SpriteConfig config);
    static i32 frame_count(const Sprite* sprite);
};

