# window/

GLFW window wrapper: owns the window handle and the options it was created
with (size, title, vsync).

| File         | Purpose |
|--------------|---------|
| `window.hpp` | `Window`/`Options` structs and function declarations (`Engine::Window`). |
| `window.cpp` | Window creation, vsync, close, event polling and buffer swap. |
