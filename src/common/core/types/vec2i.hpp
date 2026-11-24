/**
 * @file vec2i.hpp
 * @brief 2D integer vector type and its operations.
 */
#pragma once

#include "common/core/typedefs.hpp"

/// @brief Configuration used to construct a 2D integer vector.
struct Vec2iConfig {
    i32 x = 0; ///< X component.
    i32 y = 0; ///< Y component.
};

/// @brief 2D vector with integer components.
struct Vec2i {
    i32 x; ///< X component.
    i32 y; ///< Y component.

    static Vec2i create(const Vec2iConfig config);
    Vec2i add(const Vec2i b) const;
    Vec2i sub(const Vec2i b) const;
    Vec2i scale(const i32 s) const;
    bool equal(const Vec2i b) const;
    i64 dot(const Vec2i b) const;
};