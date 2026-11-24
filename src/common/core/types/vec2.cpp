/**
 * @file vec2.cpp
 * @brief Implementation of the 2D float vector type operations.
 */

#include "vec2.hpp"

#include <math.h>

/// @brief Converts degrees to radians.
/// @param degrees The angle in degrees.
/// @return The angle in radians.
static inline f32 deg_to_rad(const f32 degrees) {
    return degrees * static_cast<f32>(M_PI) / 180.0f;
}

/// @brief Creates a vector from its coordinates.
/// @param config The x and y coordinates.
/// @return The initialized vector.
Vec2 Vec2::create(const Vec2Config config) {
    return Vec2{
        .x = config.x,
        .y = config.y,
    };
}

/// @brief Adds another vector to this one.
/// @param vec The vector to add.
/// @return The sum of the two vectors.
Vec2 Vec2::add(const Vec2 vec) const {
    return Vec2{
        .x = this->x + vec.x,
        .y = this->y + vec.y,
    };
}

/// @brief Subtracts another vector from this one.
/// @param vec The vector to subtract.
/// @return The difference of the two vectors.
Vec2 Vec2::sub(const Vec2 vec) const {
    return Vec2{
        .x = this->x - vec.x,
        .y = this->y - vec.y,
    };
}

/// @brief Scales both coordinates by a factor.
/// @param scale The multiplier applied to each component.
/// @return A new vector holding the scaled coordinates.
Vec2 Vec2::scale(const f32 scale) const {
    return Vec2{
        .x = this->x * scale,
        .y = this->y * scale,
    };
}

/// @brief Computes the dot product with another vector.
/// @param vec The other vector.
/// @return The dot product.
f32 Vec2::dot(const Vec2 vec) const {
    return this->x * vec.x + this->y * vec.y;
}

/// @brief Computes the length of this vector.
/// @return The length, or empty when this is the zero vector.
std::optional<f32> Vec2::length() const {
    if (this->x == 0.0f && this->y == 0.0f) {
        return std::nullopt;
    }
    return std::sqrt(this->x * this->x + this->y * this->y);
}

/// @brief Produces a unit vector in this vector's direction.
/// @return The normalized vector, or empty when this is the zero vector.
std::optional<Vec2> Vec2::normalize() const {
    const std::optional<f32> len = this->length();
    if (!len.has_value()) {
        return std::nullopt;
    }
    return Vec2{
        .x = this->x / len.value(),
        .y = this->y / len.value(),
    };
}

/// @brief Rotates this vector about the origin.
/// @param radians The rotation angle in radians.
/// @return The rotated vector.
Vec2 Vec2::rotate(const f32 radians) const {
    return Vec2{
        .x = this->x * std::cos(radians) - this->y * std::sin(radians),
        .y = this->x * std::sin(radians) + this->y * std::cos(radians),
    };
}

/// @brief Rotates this vector about an arbitrary point.
/// @param config The pivot point and rotation angle in radians.
/// @return The rotated vector.
Vec2 Vec2::rotate_around(const RotateAroundConfig config) const {
    const Vec2 offset = this->sub(config.point);
    const Vec2 rotated = offset.rotate(config.radians);
    return rotated.add(config.point);
}

/// @brief Rotates this vector about the origin.
/// @param degrees The rotation angle in degrees.
/// @return The rotated vector.
Vec2 Vec2::rotate_degrees(const f32 degrees) const {
    return this->rotate(deg_to_rad(degrees));
}

/// @brief Rotates this vector about an arbitrary point.
/// @param config The pivot point and rotation angle in degrees.
/// @return The rotated vector.
Vec2 Vec2::rotate_around_degrees(const RotateAroundDegreesConfig config) const {
    return this->rotate_around({
        .point = config.point,
        .radians = deg_to_rad(config.degrees),
    });
}

/// @brief Linearly interpolates toward another vector.
/// @param config The target vector and interpolation factor.
/// @return The interpolated vector.
Vec2 Vec2::lerp(const LerpConfig config) const {
    return Vec2{
        .x = this->x + (config.to.x - this->x) * config.t,
        .y = this->y + (config.to.y - this->y) * config.t,
    };
}

/// @brief Computes the distance to another vector.
/// @param b The other vector.
/// @return The distance between the two vectors.
f32 Vec2::distance(const Vec2 b) const {
    const Vec2 diff = this->sub(b);
    const std::optional<f32> len = diff.length();
    return len.value_or(0.0f);
}
   
 