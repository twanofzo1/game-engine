/**
 * @file vec2.hpp
 * @brief 2D float vector type and its operations.
 */
#pragma once

#include "core/typedefs.hpp"


/** @brief Configuration used to construct a 2D float vector. */
struct Vec2Config {
    f32 x = 0.0f; ///< X component.
    f32 y = 0.0f; ///< Y component.
};

/** @brief 2D vector with float components. */
struct Vec2 {
    f32 x; ///< X component.
    f32 y; ///< Y component.

    static Vec2 make(Vec2Config config);
    static Vec2 add(Vec2 a, Vec2 b);
    static Vec2 sub(Vec2 a, Vec2 b);
    static Vec2 scale(Vec2 v, f32 s);
    static f32 dot(Vec2 a, Vec2 b);
    static f32 length(Vec2 v);
    static Vec2 normalize(Vec2 v);
    static Vec2 rotate(Vec2 v, f32 radians);
    static Vec2 lerp(Vec2 a, Vec2 b, f32 t);
    static f32 distance(Vec2 a, Vec2 b);
};

