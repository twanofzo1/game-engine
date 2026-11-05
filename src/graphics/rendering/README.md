# rendering/

OpenGL 3.3 rendering layer: camera, shaders and a 2D batch renderer.

| File             | Purpose |
|------------------|---------|
| `vertex.hpp`     | Vertex layout (position, UV, color). |
| `camera.hpp`     | Orthographic camera declaration. |
| `camera.cpp`     | Orthographic view-projection matrix. |
| `shader.hpp`     | Shader program helper declaration. |
| `shader.cpp`     | Shader compilation/linking implementation. |
| `renderer.hpp`   | 2D batch renderer declaration. |
| `renderer.cpp`   | 2D batch renderer implementation. |
| `texture.hpp`    | GPU texture: RGBA8 upload plus filter/wrap settings. |
| `texture.cpp`    | Texture creation, sampling settings and deletion. |
