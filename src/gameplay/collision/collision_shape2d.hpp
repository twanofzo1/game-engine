/**
 * @file collision_shape2d.hpp
 * @brief 2D collision shapes (circle, rect, polygon) and overlap testing.
 */
#pragma once

#include "core/typedefs.hpp"
#include "core/types/vec2.hpp"
#include "core/types/rect.hpp"

namespace Engine {
namespace Collision {

/** Shape kinds. */
enum ShapeType {
    SHAPE_CIRCLE,
    SHAPE_RECT,
    SHAPE_POLYGON
};

/** Maximum polygon vertices stored inline (keeps the shape POD / ECS-friendly). */
#define COLLISION_POLYGON_MAX_POINTS 16

/**
 * A 2D collision shape. Only the fields for the active `type` are used:
 * circle uses pos/radius, rect uses rect/rotation, polygon uses points.
 * All data is stored inline, so the struct is POD and safe to memcpy.
 */
struct CollisionShape2D {
    ShapeType type;

    /* Circle. */
    Vec2::Vec2 pos;    ///< Circle center.
    f32        radius; ///< Circle radius.

    /* Rectangle (a square is a rect with equal width/height). */
    Rect::Rect rect;     ///< Bottom-left corner + size.
    f32        rotation; ///< Rotation in radians about the rect center.

    /* Polygon: a set of points connected into a closed shape. */
    Vec2::Vec2 points[COLLISION_POLYGON_MAX_POINTS];
    u32         point_count;
};

void init(CollisionShape2D* shape);

void make_circle(CollisionShape2D* shape, Vec2::Vec2 pos, f32 radius);
void make_rect(CollisionShape2D* shape, Rect::Rect rect, f32 rotation);
void make_polygon(CollisionShape2D* shape, const Vec2::Vec2* points, u32 count);

/** @brief Axis-aligned bounding box of a shape, written to `mn`/`mx`. */
void bounds(const CollisionShape2D* shape, Vec2::Vec2* mn, Vec2::Vec2* mx);

/**
 * @brief True when two shapes overlap.
 * @warning Polygon-vs-polygon and rect-vs-polygon use SAT, which assumes the
 *          polygon is convex. Circle tests support concave polygons.
 */
bool collide(const CollisionShape2D* a, const CollisionShape2D* b);

} // namespace Collision
} // namespace Engine
