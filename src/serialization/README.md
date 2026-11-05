# serialization/

YAML config (de)serialization.

Thin facade over the vendored yaml-cpp library (`external/yaml-cpp`, MIT).
yaml-cpp is a full YAML parser/emitter (block and flow collections). The facade
adds exception-free `load`/`load_text`/`save`, typed accessors (`as_*` with
fallback, `get_*` with success flag), and `make_vec2`/`make_color` builders.
Higher-level struct serializers live in one file per struct.

| File                  | Purpose |
|-----------------------|---------|
| `yaml.hpp`            | `Yaml::Node` typedef, load/save and scalar/Vec2/Color helpers. |
| `yaml.cpp`            | Implementation of the facade. |
| `window_yaml.hpp`     | `load_window` / `make_window` for `Window::Options`. |
| `window_yaml.cpp`     | Implementation of window (de)serialization. |
| `texture_yaml.hpp`    | `load_texture` / `make_texture` for `TextureConfig`. |
| `texture_yaml.cpp`    | Implementation of texture (de)serialization. |
| `sprite_yaml.hpp`     | `load_sprite` / `make_sprite` for `SpriteConfig`. |
| `sprite_yaml.cpp`     | Implementation of sprite (de)serialization. |
| `animation_yaml.hpp`  | `load_animation` / `make_animation` for `Animation`. |
| `animation_yaml.cpp`  | Implementation of animation (de)serialization. |
| `attack_yaml.hpp`     | `load_attack` / `make_attack` for `Attack`. |
| `attack_yaml.cpp`     | Implementation of attack (de)serialization. |
| `collision_shape2d_yaml.hpp` | `load_collision_shape` / `make_collision_shape` for `CollisionShape2D`. |
| `collision_shape2d_yaml.cpp` | Implementation of collision shape (de)serialization. |
