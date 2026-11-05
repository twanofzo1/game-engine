/**
 * @file color.hpp
 * @brief RGBA color type and its operations.
 *
 * Colors are stored as normalized floats in [0, 1], matching the format the
 * renderer uploads to the GPU, so there is no per-sprite conversion cost.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Color {

/** RGBA color with normalized float components in [0, 1]. */
struct Color {
    f32 r; ///< Red component.
    f32 g; ///< Green component.
    f32 b; ///< Blue component.
    f32 a; ///< Alpha component.
};

/** @brief Construct a Color from four normalized components. */
static inline Color make(f32 r, f32 g, f32 b, f32 a)
{
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

/** @brief Linear interpolation between two colors. */
static inline Color lerp(Color a, Color b, f32 t)
{
    return make(a.r + (b.r - a.r) * t,
                a.g + (b.g - a.g) * t,
                a.b + (b.b - a.b) * t,
                a.a + (b.a - a.a) * t);
}

} // namespace Color
} // namespace Engine
