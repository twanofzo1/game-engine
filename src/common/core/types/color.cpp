/**
 * @file color.cpp
 * @brief Implementation of the RGBA color type operations.
 */
#include "color.hpp"

/** @brief Construct a color from a configuration. */
Color Color::make(ColorConfig config)
{
    return {
        .r = config.r,
        .g = config.g,
        .b = config.b,
        .a = config.a
    };
}

/** @brief Linear interpolation between two colors by `t` in [0,1]. */
Color Color::lerp(Color a, Color b, f32 t)
{
    return {
        .r = a.r + (b.r - a.r) * t,
        .g = a.g + (b.g - a.g) * t,
        .b = a.b + (b.b - a.b) * t,
        .a = a.a + (b.a - a.a) * t
    };
}

/** @brief Darken a color by blending it toward black by `factor`. */
Color Color::darken(Color c, f32 factor)
{
    return {
        .r = c.r * (1.0f - factor),
        .g = c.g * (1.0f - factor),
        .b = c.b * (1.0f - factor),
        .a = c.a
    };
}

/** @brief Lighten a color by blending it toward white by `factor`. */
Color Color::lighten(Color c, f32 factor)
{
    return {
        .r = c.r + (1.0f - c.r) * factor,
        .g = c.g + (1.0f - c.g) * factor,
        .b = c.b + (1.0f - c.b) * factor,
        .a = c.a
    };
}

/** @brief Build a color from a packed 32-bit RGBA value. */
Color Color::from_hex(u32 hex)
{
    return {
        .r = ((hex >> 24) & 0xFF) / 255.0f,
        .g = ((hex >> 16) & 0xFF) / 255.0f,
        .b = ((hex >> 8) & 0xFF) / 255.0f,
        .a = (hex & 0xFF) / 255.0f
    };
}

/** @brief Pack a color into a 32-bit RGBA value. */
u32 Color::to_hex(Color c)
{
    return ((u32)(c.r * 255.0f) << 24) |
           ((u32)(c.g * 255.0f) << 16) |
           ((u32)(c.b * 255.0f) << 8) |
           ((u32)(c.a * 255.0f));
}

/** @brief Build a color from 8-bit RGBA components in [0,255]. */
Color Color::from_8_bit(u8 r, u8 g, u8 b, u8 a)
{
    return {
        .r = r / 255.0f,
        .g = g / 255.0f,
        .b = b / 255.0f,
        .a = a / 255.0f
    };
}

/** @brief Convert a color to 8-bit RGBA components written to the output pointers. */
void Color::to_8_bit(Color c, u8* r, u8* g, u8* b, u8* a)
{
    *r = (u8)(c.r * 255.0f);
    *g = (u8)(c.g * 255.0f);
    *b = (u8)(c.b * 255.0f);
    *a = (u8)(c.a * 255.0f);
}