/**
 * @file test_sprite.cpp
 * @brief Tests for the sprite-sheet type: frame grid and UV lookup.
 */
#include "framework.hpp"

#include "core/types/sprite.hpp"
#include "core/types/vec2.hpp"

void test_sprite(void)
{
    Engine::Sprite::Sprite sprite = Engine::Sprite::make(
        7, Engine::Vec2::make(46.0f, 50.0f), 8, 4);

    /* Defaults. */
    TEST_CHECK(sprite.texture == 7);
    TEST_CHECK(sprite.frames_x == 8 && sprite.frames_y == 4);
    TEST_CHECK(sprite.frame == 0);
    TEST_CHECK_FEQ(sprite.size.x, 46.0f, 1e-6);
    TEST_CHECK_FEQ(sprite.color.r, 1.0f, 1e-6);
    TEST_CHECK(Engine::Sprite::frame_count(&sprite) == 32);

    /* UV rect of frame 0 (top-left). */
    f32 uv[4];
    Engine::Sprite::uv_rect(&sprite, uv);
    TEST_CHECK_FEQ(uv[0], 0.0f, 1e-6);
    TEST_CHECK_FEQ(uv[1], 0.0f, 1e-6);
    TEST_CHECK_FEQ(uv[2], 1.0f / 8.0f, 1e-6);
    TEST_CHECK_FEQ(uv[3], 1.0f / 4.0f, 1e-6);

    /* First frame of the second row. */
    sprite.frame = 8;
    Engine::Sprite::uv_rect(&sprite, uv);
    TEST_CHECK_FEQ(uv[0], 0.0f, 1e-6);
    TEST_CHECK_FEQ(uv[1], 1.0f / 4.0f, 1e-6);
    TEST_CHECK_FEQ(uv[2], 1.0f / 8.0f, 1e-6);
    TEST_CHECK_FEQ(uv[3], 2.0f / 4.0f, 1e-6);

    /* Last frame of the grid. */
    sprite.frame = 31;
    Engine::Sprite::uv_rect(&sprite, uv);
    TEST_CHECK_FEQ(uv[0], 7.0f / 8.0f, 1e-6);
    TEST_CHECK_FEQ(uv[1], 3.0f / 4.0f, 1e-6);
    TEST_CHECK_FEQ(uv[2], 1.0f, 1e-6);
    TEST_CHECK_FEQ(uv[3], 1.0f, 1e-6);

    /* Negative frame wraps to the end. */
    sprite.frame = -1;
    Engine::Sprite::uv_rect(&sprite, uv);
    TEST_CHECK_FEQ(uv[0], 7.0f / 8.0f, 1e-6);

    /* A single-frame grid samples the whole texture. */
    Engine::Sprite::Sprite single = Engine::Sprite::make(
        0, Engine::Vec2::make(10.0f, 10.0f), 1, 1);
    Engine::Sprite::uv_rect(&single, uv);
    TEST_CHECK_FEQ(uv[0], 0.0f, 1e-6);
    TEST_CHECK_FEQ(uv[1], 0.0f, 1e-6);
    TEST_CHECK_FEQ(uv[2], 1.0f, 1e-6);
    TEST_CHECK_FEQ(uv[3], 1.0f, 1e-6);
}
