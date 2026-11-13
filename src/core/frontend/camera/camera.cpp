/**
 * @file camera.cpp
 * @brief 2D orthographic camera matrix implementation.
 */
#include "core/frontend/camera/camera.hpp"

#include <string.h>


Camera Camera::make(const CameraConfig config)
{
    Camera camera;
    camera.position = config.position;
    camera.size = config.size;
    return camera;
}

void Camera::view_projection(const Camera* camera, f32* m)
{
    memset(m, 0, sizeof(f32) * 16);

    /* Orthographic projection mapping world space into NDC. The camera sits
     * at `position` and shows a `size` region. Column-major layout as
     * expected by glUniformMatrix4fv. */
    m[0]  =  2.0f / camera->size.x;
    m[5]  =  2.0f / camera->size.y;
    m[10] =  1.0f;
    m[15] =  1.0f;

    m[12] = -(2.0f * camera->position.x) / camera->size.x;
    m[13] = -(2.0f * camera->position.y) / camera->size.y;
    m[14] =  0.0f;
}


