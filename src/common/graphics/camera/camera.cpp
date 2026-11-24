/**
 * @file camera.cpp
 * @brief 2D orthographic camera matrix implementation.
 */
#include "common/graphics/camera/camera.hpp"

#include <string.h>

/// @brief Creates a camera from its position and size.
/// @param config The center position and visible size.
/// @return The initialized camera.
Camera Camera::create(const CameraConfig config)
{
    Camera camera;
    camera.position = config.position;
    camera.size = config.size;
    return camera;
}

/// @brief Computes the orthographic view-projection matrix into a 16-element array.
/// @param out_m16 Where the column-major 4x4 matrix is written (16 floats).
/// @warn `out_m16` must point to storage for 16 floats.
ViewProjectionMatrix Camera::compute_orthographic_projection_matrix() const
{
    ViewProjectionMatrix vp;
    memset(vp.matrix, 0, sizeof(f32) * 16);

    /* Orthographic projection mapping world space into NDC. The camera sits
     * at `position` and shows a `size` region. Column-major layout as
     * expected by glUniformMatrix4fv. */
    vp.matrix[0]  =  2.0f / this->size.x;
    vp.matrix[5]  =  2.0f / this->size.y;
    vp.matrix[10] =  1.0f;
    vp.matrix[15] =  1.0f;

    vp.matrix[12] = -(2.0f * this->position.x) / this->size.x;
    vp.matrix[13] = -(2.0f * this->position.y) / this->size.y;
    vp.matrix[14] =  0.0f;

    return vp;
}


