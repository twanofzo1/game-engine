# src/

Engine source code. Everything is C-style: plain structs and free functions,
no classes and no inheritance. Components are data, systems are functions.

Code is grouped into sub-namespaces under `Engine`: `Engine::Ecs`,
`Engine::Render`, `Engine::Input`, `Engine::Systems`, `Engine::Window`,
`Engine::MemoryPool`, `Engine::Resource`, `Engine::Image`, `Engine::Audio`,
`Engine::Texture`, `Engine::AudioManager`, `Engine::Timers`,
and one namespace per type (`Engine::Vec2`, `Engine::Rect`,
`Engine::Transform2D`, `Engine::Color`, `Engine::Sprite`, ...). Primitive aliases in
`core/typedefs.hpp` stay global.

## Layout

| Directory         | Purpose |
|-------------------|---------|
| `main.cpp`        | Demo entry point: window, GL/ImGui setup and the render loop |
| `imgui_gl_loader.h` | Forces ImGui's OpenGL3 backend to use GLAD as the single GL loader |
| `core/`           | Engine foundation: typedefs, memory pool, math/geometry types, ECS, event bus, timers |
| `platform/`       | OS/GLFW integration: window and unified input |
| `graphics/`       | OpenGL rendering: camera, shaders, textures and the 2D batch renderer |
| `resources/`      | File cache and image/audio decoders (PNG/JPG/BMP/GIF/TGA/HDR, WAV, MP3) |
| `audio/`          | Audio playback manager (miniaudio backend) |
| `systems/`        | Systems that read/write the ECS world (render and combat systems) |
| `gameplay/`       | Game logic: animation, attacks and 2D collision shapes |
| `serialization/`  | YAML facade and per-struct serializers |

## core/

| Directory     | Purpose |
|---------------|---------|
| `typedefs.hpp` | Global fixed-width primitive aliases (`u8`, `u32`, `f32`, ...) and the `Subscriber` typedef |
| `allocator/`   | Custom memory allocators (`Engine::MemoryPool`) |
| `types/`       | Math/geometry types (Vec2, Rect, Transform2D, Color, Sprite, ...) and their functions |
| `ecs/`         | Entity registry, sparse-set component storage and component definitions |
| `event/`       | Publish/subscribe event bus |
| `timers/`      | Timer system: scheduled one-shot/repeating callbacks |

## platform/

| Directory | Purpose |
|-----------|---------|
| `window/`  | GLFW window wrapper and options (size, title, vsync) |
| `input/`   | Unified keyboard + mouse + gamepad input |

## graphics/

| Directory    | Purpose |
|--------------|---------|
| `rendering/` | Camera, shaders, textures and the 2D batch renderer |

## gameplay/

| Directory    | Purpose |
|--------------|---------|
| `animation/` | Sprite animation playback (keyframes) |
| `attack/`    | Attack data: phased timing plus hitbox/event/movement keyframes |
| `collision/` | 2D collision shapes and overlap testing |

## serialization/

Flat directory: the YAML facade (`yaml.hpp`) plus one serializer per struct
(`window_yaml`, `texture_yaml`, `sprite_yaml`, `animation_yaml`, `attack_yaml`,
`collision_shape2d_yaml`).
