/**
 * @file collision_shape2d.cpp
 * @brief 2D collision shape overlap implementation.
 */
#include "gameplay/collision/collision_shape2d.hpp"

#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Collision {

/* --- construction --- */

void init(CollisionShape2D* shape)
{
    memset(shape, 0, sizeof(CollisionShape2D));
}

void make_circle(CollisionShape2D* shape, Vec2::Vec2 pos, f32 radius)
{
    init(shape);
    shape->type   = SHAPE_CIRCLE;
    shape->pos    = pos;
    shape->radius = radius < 0.0f ? 0.0f : radius;
}

void make_rect(CollisionShape2D* shape, Rect::Rect rect, f32 rotation)
{
    init(shape);
    shape->type     = SHAPE_RECT;
    shape->rect     = rect;
    shape->rotation = rotation;
}

void make_polygon(CollisionShape2D* shape, const Vec2::Vec2* points, u32 count)
{
    init(shape);
    shape->type = SHAPE_POLYGON;
    if (!points || count == 0)
        return;

    if (count > COLLISION_POLYGON_MAX_POINTS)
        count = COLLISION_POLYGON_MAX_POINTS;

    memcpy(shape->points, points, sizeof(Vec2::Vec2) * count);
    shape->point_count = count;
}

/* --- helpers --- */

static f32 clampf(f32 v, f32 lo, f32 hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void bounds(const CollisionShape2D* s, Vec2::Vec2* mn, Vec2::Vec2* mx)
{
    if (s->type == SHAPE_CIRCLE)
    {
        *mn = Vec2::make(s->pos.x - s->radius, s->pos.y - s->radius);
        *mx = Vec2::make(s->pos.x + s->radius, s->pos.y + s->radius);
    }
    else if (s->type == SHAPE_RECT)
    {
        Vec2::Vec2 c  = Rect::center(s->rect);
        f32 hw = s->rect.size.x * 0.5f;
        f32 hh = s->rect.size.y * 0.5f;
        f32 ca = fabsf(cosf(s->rotation));
        f32 sa = fabsf(sinf(s->rotation));
        f32 rx = hw * ca + hh * sa;
        f32 ry = hw * sa + hh * ca;
        *mn = Vec2::make(c.x - rx, c.y - ry);
        *mx = Vec2::make(c.x + rx, c.y + ry);
    }
    else
    {
        if (s->point_count == 0)
        {
            *mn = Vec2::make(0.0f, 0.0f);
            *mx = Vec2::make(0.0f, 0.0f);
            return;
        }
        *mn = s->points[0];
        *mx = s->points[0];
        for (u32 i = 1; i < s->point_count; ++i)
        {
            if (s->points[i].x < mn->x) mn->x = s->points[i].x;
            if (s->points[i].y < mn->y) mn->y = s->points[i].y;
            if (s->points[i].x > mx->x) mx->x = s->points[i].x;
            if (s->points[i].y > mx->y) mx->y = s->points[i].y;
        }
    }
}

/** Corners of a rotated rect, counter-clockwise. */
static void rect_corners(const Rect::Rect* r, f32 rotation, Vec2::Vec2 out[4])
{
    Vec2::Vec2 c  = Rect::center(*r);
    f32 hw = r->size.x * 0.5f;
    f32 hh = r->size.y * 0.5f;

    const Vec2::Vec2 local[4] = {
        Vec2::make(-hw, -hh),
        Vec2::make( hw, -hh),
        Vec2::make( hw,  hh),
        Vec2::make(-hw,  hh),
    };
    for (int i = 0; i < 4; ++i)
        out[i] = Vec2::add(c, Vec2::rotate(local[i], rotation));
}

/** Project a polygon onto an axis. */
static void project(const Vec2::Vec2* pts, u32 n, Vec2::Vec2 axis, f32* mn, f32* mx)
{
    f32 d = Vec2::dot(pts[0], axis);
    *mn = d;
    *mx = d;
    for (u32 i = 1; i < n; ++i)
    {
        d = Vec2::dot(pts[i], axis);
        if (d < *mn) *mn = d;
        if (d > *mx) *mx = d;
    }
}

/** Separating axis test between two convex polygons. */
static bool sat(const Vec2::Vec2* a, u32 an, const Vec2::Vec2* b, u32 bn)
{
    for (int pass = 0; pass < 2; ++pass)
    {
        const Vec2::Vec2* p = pass == 0 ? a : b;
        u32 n = pass == 0 ? an : bn;

        for (u32 i = 0; i < n; ++i)
        {
            Vec2::Vec2 e = Vec2::sub(p[(i + 1) % n], p[i]);
            Vec2::Vec2 axis = Vec2::make(-e.y, e.x);

            f32 amin, amax, bmin, bmax;
            project(a, an, axis, &amin, &amax);
            project(b, bn, axis, &bmin, &bmax);
            if (amax < bmin || bmax < amin)
                return false;
        }
    }
    return true;
}

static Vec2::Vec2 closest_on_segment(Vec2::Vec2 p, Vec2::Vec2 a, Vec2::Vec2 b)
{
    Vec2::Vec2 ab = Vec2::sub(b, a);
    f32 len2 = Vec2::dot(ab, ab);
    if (len2 <= 0.0f)
        return a;

    f32 t = clampf(Vec2::dot(Vec2::sub(p, a), ab) / len2, 0.0f, 1.0f);
    return Vec2::add(a, Vec2::scale(ab, t));
}

static bool point_in_polygon(Vec2::Vec2 p, const Vec2::Vec2* pts, u32 n)
{
    bool inside = false;
    for (u32 i = 0, j = n - 1; i < n; j = i++)
    {
        if (((pts[i].y > p.y) != (pts[j].y > p.y)) &&
            (p.x < (pts[j].x - pts[i].x) * (p.y - pts[i].y) / (pts[j].y - pts[i].y) + pts[i].x))
            inside = !inside;
    }
    return inside;
}

static bool circle_circle(Vec2::Vec2 p1, f32 r1, Vec2::Vec2 p2, f32 r2)
{
    f32 r = r1 + r2;
    Vec2::Vec2 d = Vec2::sub(p1, p2);
    return Vec2::dot(d, d) <= r * r;
}

static bool circle_rect(Vec2::Vec2 center, f32 radius, const Rect::Rect* rect, f32 rotation)
{
    Vec2::Vec2 local = Vec2::rotate(Vec2::sub(center, Rect::center(*rect)), -rotation);

    f32 hw = rect->size.x * 0.5f;
    f32 hh = rect->size.y * 0.5f;
    f32 dx = local.x - clampf(local.x, -hw, hw);
    f32 dy = local.y - clampf(local.y, -hh, hh);
    return dx * dx + dy * dy <= radius * radius;
}

static bool circle_polygon(Vec2::Vec2 center, f32 radius, const Vec2::Vec2* pts, u32 n)
{
    if (point_in_polygon(center, pts, n))
        return true;

    f32 r2 = radius * radius;
    for (u32 i = 0; i < n; ++i)
    {
        Vec2::Vec2 c = closest_on_segment(center, pts[i], pts[(i + 1) % n]);
        Vec2::Vec2 d = Vec2::sub(center, c);
        if (Vec2::dot(d, d) <= r2)
            return true;
    }
    return false;
}

/* --- public collide --- */

bool collide(const CollisionShape2D* a, const CollisionShape2D* b)
{
    if (!a || !b)
        return false;
    if (a->type == SHAPE_POLYGON && a->point_count < 3)
        return false;
    if (b->type == SHAPE_POLYGON && b->point_count < 3)
        return false;

    /* Cheap AABB reject before the narrow phase. */
    Vec2::Vec2 amin, amax, bmin, bmax;
    bounds(a, &amin, &amax);
    bounds(b, &bmin, &bmax);
    if (amax.x < bmin.x || bmax.x < amin.x ||
        amax.y < bmin.y || bmax.y < amin.y)
        return false;

    /* Normalize so the "simpler" shape (lower type id) is `a`. */
    if (b->type < a->type)
    {
        const CollisionShape2D* t = a;
        a = b;
        b = t;
    }

    switch (a->type)
    {
        case SHAPE_CIRCLE:
            if (b->type == SHAPE_CIRCLE)
                return circle_circle(a->pos, a->radius, b->pos, b->radius);
            if (b->type == SHAPE_RECT)
                return circle_rect(a->pos, a->radius, &b->rect, b->rotation);
            return circle_polygon(a->pos, a->radius, b->points, b->point_count);

        case SHAPE_RECT:
        {
            Vec2::Vec2 av[4], bv[4];
            rect_corners(&a->rect, a->rotation, av);
            if (b->type == SHAPE_RECT)
            {
                rect_corners(&b->rect, b->rotation, bv);
                return sat(av, 4, bv, 4);
            }
            return sat(av, 4, b->points, b->point_count);
        }

        case SHAPE_POLYGON:
            return sat(a->points, a->point_count, b->points, b->point_count);

        default:
            return false;
    }
}

} // namespace Collision
} // namespace Engine
