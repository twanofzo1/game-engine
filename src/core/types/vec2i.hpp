/**
 * @file vec2i.hpp
 * @brief 2D integer vector type and its operations.
 */
#pragma once

#include "core/typedefs.hpp"


/** @brief Configuration used to construct a 2D integer vector. */
struct Vec2iConfig {
    i32 x = 0; ///< X component.
    i32 y = 0; ///< Y component.
};

/** @brief 2D vector with integer components. */
struct Vec2i {
    i32 x; ///< X component.
    i32 y; ///< Y component.

    static Vec2i make(Vec2iConfig config);
    static Vec2i add(Vec2i a, Vec2i b);
    static Vec2i sub(Vec2i a, Vec2i b);
    static Vec2i scale(Vec2i v, i32 s);
    static bool equal(Vec2i a, Vec2i b);
    static i64 dot(Vec2i a, Vec2i b);
};