# rendering/

OpenGL 3.3 rendering layer: camera, shaders and a 2D batch renderer.

| File             | Purpose |
|------------------|---------|
| `vertex.hpp`     | Vertex layout (position, UV, color). |
| `camera.hpp`     | Orthographic camera declaration. |
| `camera.cpp`     | Orthographic view-projection matrix. |
| `shader.hpp`     | Shader program helper declaration. |
| `shader.cpp`     | Shader compilation/linking implementation. |
| `renderer.hpp`   | Multi-shader 2D batch renderer declaration. |
| `renderer.cpp`   | Renderer public API (make/shutdown/begin/push/end). |
| `renderer_internal.hpp` | Internal helpers shared across the renderer's translation units. |
| `renderer_batch.cpp` | CPU-side sorting and draw-call batching. |
| `renderer_gl.cpp` | OpenGL resource creation and draw submission. |
| `texture.hpp`    | GPU texture: RGBA8 upload plus filter/wrap settings. |
| `texture.cpp`    | Texture creation, sampling settings and deletion. |
