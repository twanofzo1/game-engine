# collision/

2D collision shapes and overlap testing. A shape is a circle, a (possibly
rotated) rectangle, or a polygon made of connected points. All shape data is
stored inline (POD), so shapes are safe to memcpy into ECS components.
`collide` uses an AABB pre-check followed by a narrow phase (distance tests
for circles, SAT for convex polygon pairs).

| File                    | Purpose |
|-------------------------|---------|
| `collision_shape2d.hpp` | `CollisionShape2D` type, constructors, `bounds` and `collide`. |
| `collision_shape2d.cpp` | Overlap tests (AABB reject, circle tests, SAT) and AABB bounds. |
