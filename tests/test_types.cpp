/**
 * @file test_types.cpp
 * @brief Tests for the math/geometry types in src/core/types/.
 */
#include "framework.hpp"

#include "core/types/vec2.hpp"
#include "core/types/vec2i.hpp"
#include "core/types/rect.hpp"
#include "core/types/recti.hpp"
#include "core/types/color.hpp"
#include "core/types/transform2d.hpp"
#include "core/types/transform2di.hpp"

#define TEST_PI_HALF 1.57079632679f

void test_types(void)
{
    /* Vec2 arithmetic. */
    {
        Engine::Vec2::Vec2 a = Engine::Vec2::make(2.0f, 3.0f);
        Engine::Vec2::Vec2 b = Engine::Vec2::make(4.0f, -1.0f);

        Engine::Vec2::Vec2 sum = Engine::Vec2::add(a, b);
        TEST_CHECK_FEQ(sum.x, 6.0f, 1e-6);
        TEST_CHECK_FEQ(sum.y, 2.0f, 1e-6);

        Engine::Vec2::Vec2 diff = Engine::Vec2::sub(a, b);
        TEST_CHECK_FEQ(diff.x, -2.0f, 1e-6);
        TEST_CHECK_FEQ(diff.y, 4.0f, 1e-6);

        Engine::Vec2::Vec2 sc = Engine::Vec2::scale(a, 2.0f);
        TEST_CHECK_FEQ(sc.x, 4.0f, 1e-6);
        TEST_CHECK_FEQ(sc.y, 6.0f, 1e-6);

        TEST_CHECK_FEQ(Engine::Vec2::dot(a, b), 5.0f, 1e-6);
        TEST_CHECK_FEQ(Engine::Vec2::length(Engine::Vec2::make(3.0f, 4.0f)), 5.0f, 1e-6);

        Engine::Vec2::Vec2 n = Engine::Vec2::normalize(Engine::Vec2::make(3.0f, 4.0f));
        TEST_CHECK_FEQ(n.x, 0.6f, 1e-6);
        TEST_CHECK_FEQ(n.y, 0.8f, 1e-6);

        Engine::Vec2::Vec2 r = Engine::Vec2::rotate(Engine::Vec2::make(1.0f, 0.0f), TEST_PI_HALF);
        TEST_CHECK_FEQ(r.x, 0.0f, 1e-5);
        TEST_CHECK_FEQ(r.y, 1.0f, 1e-5);

        Engine::Vec2::Vec2 l = Engine::Vec2::lerp(Engine::Vec2::make(0.0f, 0.0f),
                                                   Engine::Vec2::make(10.0f, 20.0f), 0.5f);
        TEST_CHECK_FEQ(l.x, 5.0f, 1e-6);
        TEST_CHECK_FEQ(l.y, 10.0f, 1e-6);
    }

    /* Vec2i arithmetic. */
    {
        Engine::Vec2i::Vec2i a = Engine::Vec2i::make(2, 3);
        Engine::Vec2i::Vec2i b = Engine::Vec2i::make(4, -1);

        Engine::Vec2i::Vec2i sum = Engine::Vec2i::add(a, b);
        TEST_CHECK(sum.x == 6 && sum.y == 2);

        TEST_CHECK(Engine::Vec2i::equal(a, Engine::Vec2i::make(2, 3)));
        TEST_CHECK(!Engine::Vec2i::equal(a, b));
        TEST_CHECK(Engine::Vec2i::dot(a, b) == 5);

        Engine::Vec2i::Vec2i sc = Engine::Vec2i::scale(a, 3);
        TEST_CHECK(sc.x == 6 && sc.y == 9);
    }

    /* Rect edges, center, containment and intersection. */
    {
        Engine::Rect::Rect r = Engine::Rect::make(0.0f, 0.0f, 10.0f, 20.0f);
        TEST_CHECK_FEQ(Engine::Rect::left(r), 0.0f, 1e-6);
        TEST_CHECK_FEQ(Engine::Rect::right(r), 10.0f, 1e-6);
        TEST_CHECK_FEQ(Engine::Rect::bottom(r), 0.0f, 1e-6);
        TEST_CHECK_FEQ(Engine::Rect::top(r), 20.0f, 1e-6);

        Engine::Vec2::Vec2 c = Engine::Rect::center(r);
        TEST_CHECK_FEQ(c.x, 5.0f, 1e-6);
        TEST_CHECK_FEQ(c.y, 10.0f, 1e-6);

        TEST_CHECK(Engine::Rect::contains_point(r, Engine::Vec2::make(5.0f, 5.0f)));
        TEST_CHECK(Engine::Rect::contains_point(r, Engine::Vec2::make(0.0f, 0.0f)));
        TEST_CHECK(!Engine::Rect::contains_point(r, Engine::Vec2::make(10.1f, 0.0f)));

        TEST_CHECK(Engine::Rect::intersects(r, Engine::Rect::make(5.0f, 10.0f, 10.0f, 10.0f)));
        TEST_CHECK(!Engine::Rect::intersects(r, Engine::Rect::make(10.0f, 0.0f, 5.0f, 5.0f)));
    }

    /* Recti mirrors Rect with integer coordinates. */
    {
        Engine::Recti::Recti r = Engine::Recti::make(1, 2, 8, 6);
        TEST_CHECK(Engine::Recti::left(r) == 1);
        TEST_CHECK(Engine::Recti::right(r) == 9);
        TEST_CHECK(Engine::Recti::bottom(r) == 2);
        TEST_CHECK(Engine::Recti::top(r) == 8);

        Engine::Vec2i::Vec2i c = Engine::Recti::center(r);
        TEST_CHECK(c.x == 5 && c.y == 5);

        TEST_CHECK(Engine::Recti::contains_point(r, Engine::Vec2i::make(1, 2)));
        TEST_CHECK(!Engine::Recti::contains_point(r, Engine::Vec2i::make(10, 8)));
        TEST_CHECK(Engine::Recti::intersects(r, Engine::Recti::make(5, 5, 4, 4)));
        TEST_CHECK(!Engine::Recti::intersects(r, Engine::Recti::make(9, 2, 2, 2)));
    }

    /* Color: normalized float components and interpolation. */
    {
        Engine::Color::Color c = Engine::Color::make(1.0f, 0.5f, 0.0f, 1.0f);
        TEST_CHECK_FEQ(c.r, 1.0f, 1e-6);
        TEST_CHECK_FEQ(c.g, 0.5f, 1e-6);
        TEST_CHECK_FEQ(c.b, 0.0f, 1e-6);
        TEST_CHECK_FEQ(c.a, 1.0f, 1e-6);

        Engine::Color::Color mid = Engine::Color::lerp(
            Engine::Color::make(0.0f, 0.0f, 0.0f, 0.0f),
            Engine::Color::make(1.0f, 1.0f, 1.0f, 1.0f), 0.5f);
        TEST_CHECK_FEQ(mid.r, 0.5f, 1e-6);
        TEST_CHECK_FEQ(mid.g, 0.5f, 1e-6);
        TEST_CHECK_FEQ(mid.b, 0.5f, 1e-6);
        TEST_CHECK_FEQ(mid.a, 0.5f, 1e-6);
    }

    /* Transform2D: translate, scale, rotate. */
    {
        Engine::Transform2D::Transform2D t =
            Engine::Transform2D::make(Engine::Vec2::make(10.0f, 20.0f), 0.0f,
                                      Engine::Vec2::make(1.0f, 1.0f));
        Engine::Vec2::Vec2 p = Engine::Transform2D::apply(t, Engine::Vec2::make(1.0f, 2.0f));
        TEST_CHECK_FEQ(p.x, 11.0f, 1e-6);
        TEST_CHECK_FEQ(p.y, 22.0f, 1e-6);

        Engine::Transform2D::Transform2D scaled =
            Engine::Transform2D::make(Engine::Vec2::make(0.0f, 0.0f), 0.0f,
                                      Engine::Vec2::make(2.0f, 3.0f));
        p = Engine::Transform2D::apply(scaled, Engine::Vec2::make(1.0f, 1.0f));
        TEST_CHECK_FEQ(p.x, 2.0f, 1e-6);
        TEST_CHECK_FEQ(p.y, 3.0f, 1e-6);

        Engine::Transform2D::Transform2D rotated =
            Engine::Transform2D::make(Engine::Vec2::make(0.0f, 0.0f), TEST_PI_HALF,
                                      Engine::Vec2::make(1.0f, 1.0f));
        p = Engine::Transform2D::apply(rotated, Engine::Vec2::make(1.0f, 0.0f));
        TEST_CHECK_FEQ(p.x, 0.0f, 1e-5);
        TEST_CHECK_FEQ(p.y, 1.0f, 1e-5);
    }

    /* Transform2Di: integer translate/scale with rounding rotation. */
    {
        Engine::Transform2Di::Transform2Di t =
            Engine::Transform2Di::make(Engine::Vec2i::make(10, 20), 0.0f,
                                       Engine::Vec2i::make(1, 1));
        Engine::Vec2i::Vec2i p = Engine::Transform2Di::apply(t, Engine::Vec2i::make(1, 2));
        TEST_CHECK(p.x == 11 && p.y == 22);

        Engine::Transform2Di::Transform2Di scaled =
            Engine::Transform2Di::make(Engine::Vec2i::make(0, 0), 0.0f,
                                       Engine::Vec2i::make(2, 3));
        p = Engine::Transform2Di::apply(scaled, Engine::Vec2i::make(3, 4));
        TEST_CHECK(p.x == 6 && p.y == 12);
    }
}
