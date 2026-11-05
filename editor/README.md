# editor/

Standalone YAML editor executable. It edits the engine's YAML config files
through a dockspace of dockable windows: **Assets** (image tree), **Sprites**
(sprite folders, their animations and attacks), **Viewport** (sprite preview
with direct hitbox/movement editing), **Options** (Godot-style inspector),
**Animations** (animation timeline) and **Attacks** (frame-based attack
timeline with movement/hitbox/frame lanes).

The editor uses a state machine (sprite / animation / attack), a
save-or-discard guard on unsaved changes, error popups, notification toasts,
and Ctrl+Z / Ctrl+Shift+Z undo/redo.

| Folder      | Purpose |
|-------------|---------|
| `main.cpp`  | Entry point: GLFW window, Dear ImGui setup, dockspace layout and main loop |
| `core/`     | `Editor` state and sprite preview helpers (`editor.hpp/cpp`) |
| `config/`   | Reusable YAML sequence helpers (`config_helpers.hpp/cpp`) |
| `editors/`  | Per-asset editors: animations, attacks and collision shapes |
| `services/` | errors, notifications, guard, drag-and-drop and undo/redo |
| `ui/`       | ImGui panels: assets, sprites, viewport, options and timelines |
| `viewport/` | Direct viewport manipulation of hitboxes and movement |

Build with `scons editor` (optionally `scons build=debug editor`); the binary
is `build/<build>/editor`.
