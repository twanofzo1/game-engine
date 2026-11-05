# input/

A single unified input module covering keyboard, mouse, gamepad and named
actions.

| File         | Purpose |
|--------------|---------|
| `input.hpp`  | `Input` state struct, `Action` binding and inline query helpers. |
| `input.cpp`  | GLFW callbacks, frame rollover, gamepad polling and action lookups. |

Usage: call `input_init()` once (before Dear ImGui's GLFW backend so ImGui chains
the callbacks), then `input_begin_frame()` at the top of every frame before
`glfwPollEvents()`. Keys can be queried directly (`is_key_down`) or grouped into
named actions (`is_action_down` / `is_action_pressed` / `is_action_released`).
