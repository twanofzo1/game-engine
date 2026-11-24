/**
 * @file color.hpp
 * @brief RGBA color type and its operations.
 */
#pragma once
#include "common/core/typedefs.hpp"

/// @brief Configuration used to construct a color.
struct ColorConfig {
    f32 r = 0.0f; ///< Red component.
    f32 g = 0.0f; ///< Green component.
    f32 b = 0.0f; ///< Blue component.
    f32 a = 0.0f; ///< Alpha component.
};

/// @brief RGBA color with normalized float components in [0,1].
struct Color {
    f32 r; ///< Red component.
    f32 g; ///< Green component.
    f32 b; ///< Blue component.
    f32 a; ///< Alpha component.

    static Color create(const ColorConfig config);
    static Color from_hex(const u32 hex);
    static Color from_8_bit(const u8 r, const u8 g, const u8 b, const u8 a);
    Color lerp(const Color b, const f32 t) const;
    Color darken(const f32 factor) const;
    Color lighten(const f32 factor) const;
    u32 to_hex() const;
    void to_8_bit(u8* r, u8* g, u8* b, u8* a) const;
};