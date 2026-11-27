/**
 * @file vec2.hpp
 * @brief 2D float vector type and its operations.
 */
#pragma once

#include "common/core/typedefs.hpp"

struct Vec2Config;
struct RotateAroundConfig;
struct RotateAroundDegreesConfig;
struct LerpConfig;

/// @brief A 2D float vector type.
struct Vec2 {
    f32 x = 0.0f;
    f32 y = 0.0f;

    static Vec2 create(const Vec2Config config);

    Vec2 add(const Vec2 vec) const;
    Vec2 sub(const Vec2 vec) const;
    Vec2 scale(const f32 scale) const;
    f32 dot(const Vec2 vec) const;
    std::optional<f32> length() const;
    std::optional<Vec2> normalize() const;
    Vec2 rotate(const f32 radians) const;
    Vec2 rotate_around(const RotateAroundConfig config) const;
    Vec2 rotate_degrees(const f32 degrees) const;
    Vec2 rotate_around_degrees(const RotateAroundDegreesConfig config) const;
    Vec2 lerp(const LerpConfig config) const;
    f32 distance(const Vec2 b) const;
};

/// @brief Configuration used to construct a 2D float vector.
struct Vec2Config {
    f32 x = 0.0f; ///< X component.
    f32 y = 0.0f; ///< Y component.
};

/// @brief Configuration used to rotate a 2D float vector around a point.
struct RotateAroundConfig {
    Vec2 point = {.x = 0.0f, .y = 0.0f};
    f32 radians = 0;
};

/// @brief Configuration used to rotate a 2D float vector around a point in degrees.
struct RotateAroundDegreesConfig {
    Vec2 point = {.x = 0.0f, .y = 0.0f};
    f32 degrees = 0;
};

/// @brief Configuration used to linearly interpolate between two 2D float vectors.
struct LerpConfig {
    Vec2 to = {.x = 0.0f, .y = 0.0f};
    f32 t = 0;
};



