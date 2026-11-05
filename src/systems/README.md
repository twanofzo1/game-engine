# systems/

Systems hold all engine behavior. They iterate the ECS world and mutate or
draw entities; they own no persistent data beyond local scratch state.

| File                 | Purpose |
|----------------------|---------|
| `render_system.hpp`  | Declaration of the sprite rendering system. |
| `render_system.cpp`  | Draws every entity that has a Transform + Sprite. |
| `combat_system.hpp`  | Declaration of the combat/trigger system. |
| `combat_system.cpp`  | Resolves hitbox-hurtbox and area-area overlaps, honoring per-component trigger modes. |
