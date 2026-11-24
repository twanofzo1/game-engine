/**
 * @file color.cpp
 * @brief Implementation of the RGBA color type operations.
 */
#include "color.hpp"

#include <cassert>

/// @brief Constructs a color from its components.
/// @param config The RGBA components.
/// @return The initialized color.
Color Color::create(const ColorConfig config)
{
    return {
        .r = config.r,
        .g = config.g,
        .b = config.b,
        .a = config.a,
    };
}

/// @brief Linearly interpolates from this color toward another.
/// @param b The end color.
/// @param t The interpolation factor in [0,1].
/// @return The interpolated color.
Color Color::lerp(const Color b, const f32 t) const
{
    return {
        .r = this->r + (b.r - this->r) * t,
        .g = this->g + (b.g - this->g) * t,
        .b = this->b + (b.b - this->b) * t,
        .a = this->a + (b.a - this->a) * t,
    };
}

/// @brief Darkens this color by blending it toward black.
/// @param factor The blend factor.
/// @return The darkened color.
Color Color::darken(const f32 factor) const
{
    return {
        .r = this->r * (1.0f - factor),
        .g = this->g * (1.0f - factor),
        .b = this->b * (1.0f - factor),
        .a = this->a,
    };
}

/// @brief Lightens this color by blending it toward white.
/// @param factor The blend factor.
/// @return The lightened color.
Color Color::lighten(const f32 factor) const
{
    return {
        .r = this->r + (1.0f - this->r) * factor,
        .g = this->g + (1.0f - this->g) * factor,
        .b = this->b + (1.0f - this->b) * factor,
        .a = this->a,
    };
}

/// @brief Builds a color from a packed 32-bit RGBA value.
/// @param hex The packed RGBA value.
/// @return The decoded color.
Color Color::from_hex(const u32 hex)
{
    return {
        .r = ((hex >> 24) & 0xFF) / 255.0f,
        .g = ((hex >> 16) & 0xFF) / 255.0f,
        .b = ((hex >> 8) & 0xFF) / 255.0f,
        .a = (hex & 0xFF) / 255.0f,
    };
}

/// @brief Packs this color into a 32-bit RGBA value.
/// @return The packed RGBA value.
u32 Color::to_hex() const
{
    return ((u32)(this->r * 255.0f) << 24) |
           ((u32)(this->g * 255.0f) << 16) |
           ((u32)(this->b * 255.0f) << 8) |
           ((u32)(this->a * 255.0f));
}

/// @brief Builds a color from 8-bit RGBA components in [0,255].
/// @param r The red component.
/// @param g The green component.
/// @param b The blue component.
/// @param a The alpha component.
/// @return The initialized color with normalized float components.
Color Color::from_8_bit(const u8 r, const u8 g, const u8 b, const u8 a)
{
    return {
        .r = r / 255.0f,
        .g = g / 255.0f,
        .b = b / 255.0f,
        .a = a / 255.0f,
    };
}

/// @brief Converts this color to 8-bit RGBA components written to the output pointers.
/// @param r Where the red component is written.
/// @param g Where the green component is written.
/// @param b Where the blue component is written.
/// @param a Where the alpha component is written.
/// @warn The output pointers must each address a writable u8.
void Color::to_8_bit(u8* r, u8* g, u8* b, u8* a) const
{
    assert(r != nullptr);
    assert(g != nullptr);
    assert(b != nullptr);
    assert(a != nullptr);
    *r = (u8)(this->r * 255.0f);
    *g = (u8)(this->g * 255.0f);
    *b = (u8)(this->b * 255.0f);
    *a = (u8)(this->a * 255.0f);
}