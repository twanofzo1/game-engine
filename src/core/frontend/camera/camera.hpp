/**
 * @file camera.hpp
 * @brief Declares the 2D orthographic camera.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"



/** Orthographic camera describing a visible rectangle of world space. */
struct Camera {
    Vec2 position; ///< Center of the visible region in world units.
    Vec2 size;     ///< Visible width and height in world units.

    static Camera make(Vec2 position, Vec2 size);
    static void view_projection(const Camera* camera, f32* out_m16);
};

