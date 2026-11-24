/**
 * @file vec2i.cpp
 * @brief Implementation of the 2D integer vector type operations.
 */

#include "vec2i.hpp"

/// @brief Constructs a Vec2i from its coordinates.
/// @param config The x and y coordinates.
/// @return The initialized vector.
Vec2i Vec2i::create(const Vec2iConfig config)
{
    return Vec2i{
        .x = config.x,
        .y = config.y,
    };
}

/// @brief Adds a vector to this one component-wise.
/// @param b The vector to add.
/// @return The component-wise sum.
Vec2i Vec2i::add(const Vec2i b) const
{
    return Vec2i::create({.x = this->x + b.x, .y = this->y + b.y});
}

/// @brief Subtracts `b` from this vector component-wise.
/// @param b The vector to subtract.
/// @return The component-wise difference.
Vec2i Vec2i::sub(const Vec2i b) const
{
    return Vec2i::create({.x = this->x - b.x, .y = this->y - b.y});
}

/// @brief Scales this vector by an integer scalar.
/// @param s The scalar multiplier.
/// @return The scaled vector.
Vec2i Vec2i::scale(const i32 s) const
{
    return Vec2i::create({.x = this->x * s, .y = this->y * s});
}

/// @brief Returns true when both components are equal.
/// @param b The vector to compare against.
/// @return True when the vectors are component-wise equal.
bool Vec2i::equal(const Vec2i b) const
{
    return this->x == b.x && this->y == b.y;
}

/// @brief Computes the dot product with another vector using 64-bit arithmetic to avoid overflow.
/// @param b The other vector.
/// @return The dot product.
i64 Vec2i::dot(const Vec2i b) const
{
    return (i64)this->x * b.x + (i64)this->y * b.y;
}