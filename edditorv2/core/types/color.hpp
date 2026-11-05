#pragma once
#include "core/typedefs.hpp"

/** Configuration used to construct a color. */
struct ColorConfig {
    f32 r = 0.0f; ///< Red component.
    f32 g = 0.0f; ///< Green component.
    f32 b = 0.0f; ///< Blue component.
    f32 a = 0.0f; ///< Alpha component.
};

/** RGBA color with normalized float components in [0,1]. */
struct Color {
    f32 r; ///< Red component.
    f32 g; ///< Green component.
    f32 b; ///< Blue component.
    f32 a; ///< Alpha component.

    static Color make(ColorConfig config);
    static Color lerp(Color a, Color b, f32 t);
    static Color darken(Color c, f32 factor);
    static Color lighten(Color c, f32 factor);
    static Color from_hex(u32 hex);
    static u32 to_hex(Color c);
    static Color from_8_bit(u8 r, u8 g, u8 b, u8 a);
    static void to_8_bit(Color c, u8* r, u8* g, u8* b, u8* a);
};