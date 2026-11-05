/**
 * @file camera.hpp
 * @brief Declares the 2D orthographic camera.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"

namespace Engine {
namespace Render {

/** Orthographic camera describing a visible rectangle of world space. */
struct Camera {
    Vec2::Vec2 position; ///< Center of the visible region in world units.
    Vec2::Vec2 size;     ///< Visible width and height in world units.
};

/** @brief Construct a Camera from a center position and a visible size. */
static inline Camera camera_make(Vec2::Vec2 position, Vec2::Vec2 size)
{
    Camera camera;
    camera.position = position;
    camera.size     = size;
    return camera;
}

/**
 * @brief Compute a column-major orthographic view-projection matrix.
 * @param camera   Camera to project from.
 * @param out_m16  Destination array of 16 floats (column-major).
 */
void camera_view_projection(const Camera* camera, f32* out_m16);

} // namespace Render
} // namespace Engine
