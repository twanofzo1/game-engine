/**
 * @file camera.hpp
 * @brief Declares the 2D orthographic camera.
 */
#pragma once

#include "common/core/typedefs.hpp"
#include "common/core/types/vec2.hpp"

/// @brief 4x4 view-projection matrix for a camera.
struct ViewProjectionMatrix {
    f32 matrix[16]; ///< Column-major 4x4 matrix.
};


/// @brief Configuration used to construct a 2D orthographic camera.
struct CameraConfig {
    Vec2 position = Vec2::create({.x = 0.0f, .y = 0.0f}); ///< Center of the visible region in world units.
    Vec2 size = Vec2::create({.x = 1280.0f, .y = 720.0f}); ///< Visible width and height in world units.
};

/// @brief Orthographic camera describing a visible rectangle of world space.
struct Camera {
    Vec2 position; ///< Center of the visible region in world units.
    Vec2 size;     ///< Visible width and height in world units.

    static Camera create(const CameraConfig config);

    ViewProjectionMatrix compute_orthographic_projection_matrix() const;
};

