# types/

Math and geometry types used across the engine. Each type lives in its own
namespace named after the struct (rule 15): `Engine::Vec2`, `Engine::Vec2i`,
`Engine::Rect`, `Engine::Recti`, `Engine::Transform2D`, `Engine::Transform2Di`,
`Engine::Color` and `Engine::Sprite`. Functions are short (`make`, `add`,
`scale`, ...) and are
`static inline` in their headers.

| File              | Purpose |
|-------------------|---------|
| `vec2.hpp`        | `Vec2` (float 2D vector) and operations. |
| `vec2i.hpp`       | `Vec2i` (integer 2D vector) and operations. |
| `rect.hpp`        | `Rect` (float AABB) and operations. |
| `recti.hpp`       | `Recti` (integer AABB) and operations. |
| `transform2d.hpp` | `Transform2D` (float 2D transform) and operations. |
| `transform2di.hpp`| `Transform2Di` (integer 2D transform) and operations. |
| `color.hpp`       | `Color` (RGBA floats) and operations. |
| `sprite.hpp`      | `Sprite` (sprite sheet: frame grid and UV lookup). |
