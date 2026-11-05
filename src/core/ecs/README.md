# ecs/

Entity-Component-System core. Entities are lightweight handles, components are
plain data, and systems (in `../systems`) hold all behavior.

| File                 | Purpose |
|----------------------|---------|
| `components.hpp`     | Component typedefs (`Transform`, `Sprite`), the `ComponentType` enum and the `COMPONENT_INFO` declaration. |
| `components.cpp`     | Registry table listing each component's name and size (single source of truth). |
| `component_pool.hpp` | Declaration of the sparse-set component storage. |
| `component_pool.cpp` | Implementation of the sparse-set component storage. |
| `world.hpp`          | Declaration of the entity registry and world. |
| `world.cpp`          | Entity lifecycle and component access implementation. |
