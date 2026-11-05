# core/

Engine foundation: pure logic and data with no platform or OpenGL dependency.

| File / Directory | Purpose |
|------------------|---------|
| `typedefs.hpp`   | Global fixed-width primitive aliases (`u8`, `u32`, `f32`, ...) and the `Subscriber` callback typedef. |
| `allocator/`     | Custom memory allocators (`Engine::MemoryPool`). |
| `types/`         | Math/geometry types (Vec2, Rect, Transform2D, Color, Sprite, ...). |
| `ecs/`           | Entity registry, sparse-set component storage and component definitions. |
| `event/`         | Publish/subscribe event bus. |
| `timers/`        | Timer system: scheduled one-shot/repeating callbacks. |
