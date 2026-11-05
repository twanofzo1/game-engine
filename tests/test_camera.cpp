/**
 * @file test_camera.cpp
 * @brief Tests for the orthographic camera view-projection matrix.
 */
#include "framework.hpp"

#include "graphics/rendering/camera.hpp"
#include "core/types/vec2.hpp"

void test_camera(void)
{
    Engine::Render::Camera cam = Engine::Render::camera_make(
        Engine::Vec2::make(0.0f, 0.0f), Engine::Vec2::make(2.0f, 2.0f));
    TEST_CHECK_FEQ(cam.position.x, 0.0f, 1e-6);
    TEST_CHECK_FEQ(cam.size.x, 2.0f, 1e-6);

    f32 m[16];
    Engine::Render::camera_view_projection(&cam, m);

    /* Identity view at size (2,2): scale 1 on the diagonal, no translation. */
    TEST_CHECK_FEQ(m[0], 1.0f, 1e-6);
    TEST_CHECK_FEQ(m[5], 1.0f, 1e-6);
    TEST_CHECK_FEQ(m[10], 1.0f, 1e-6);
    TEST_CHECK_FEQ(m[15], 1.0f, 1e-6);
    TEST_CHECK_FEQ(m[12], 0.0f, 1e-6);
    TEST_CHECK_FEQ(m[13], 0.0f, 1e-6);

    /* Off-diagonal entries are zero (pure orthographic). */
    TEST_CHECK_FEQ(m[1], 0.0f, 1e-6);
    TEST_CHECK_FEQ(m[2], 0.0f, 1e-6);
    TEST_CHECK_FEQ(m[3], 0.0f, 1e-6);
    TEST_CHECK_FEQ(m[4], 0.0f, 1e-6);

    /* A larger visible region scales the projection down. */
    Engine::Render::Camera cam2 = Engine::Render::camera_make(
        Engine::Vec2::make(0.0f, 0.0f), Engine::Vec2::make(4.0f, 8.0f));
    Engine::Render::camera_view_projection(&cam2, m);
    TEST_CHECK_FEQ(m[0], 0.5f, 1e-6);
    TEST_CHECK_FEQ(m[5], 0.25f, 1e-6);

    /* A shifted camera translates the view. */
    Engine::Render::Camera cam3 = Engine::Render::camera_make(
        Engine::Vec2::make(3.0f, -2.0f), Engine::Vec2::make(2.0f, 2.0f));
    Engine::Render::camera_view_projection(&cam3, m);
    TEST_CHECK_FEQ(m[12], -3.0f, 1e-6);
    TEST_CHECK_FEQ(m[13], 2.0f, 1e-6);
}
