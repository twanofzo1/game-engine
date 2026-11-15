/**
 * @file camera.hpp
 * @brief Declares the 2D orthographic camera.
 */
#pragma once

#include "common/core/typedefs.hpp"
#include "common/core/types/vec2.hpp"

struct CameraConfig {
    Vec2 position = Vec2::make({.x = 0.0f, .y = 0.0f}); ///< Center of the visible region in world units.
    Vec2 size = Vec2::make({.x = 1280.0f, .y = 720.0f});     ///< Visible width and height in world units.
};

/** Orthographic camera describing a visible rectangle of world space. */
struct Camera {
    Vec2 position; ///< Center of the visible region in world units.
    Vec2 size;     ///< Visible width and height in world units.

    static Camera make(const CameraConfig config);
    static void view_projection(const Camera* camera, f32* out_m16);
};

