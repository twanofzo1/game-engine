# rendering/

OpenGL 3.3 2D batch renderer. Quads are staged in a CPU buffer across the
frame, sorted by (shader, texture), then drawn with one call per group.

| File                    | Purpose |
|-------------------------|---------|
| `vertex.hpp`            | Vertex layout (position, UV, RGBA color). |
| `renderer.hpp`          | 2D batch renderer declaration and public API. |
| `renderer.cpp`          | Public API (create/destroy/begin/push_quad/end) and shader setup. |
| `renderer_internal.hpp` | Cross-file internal helpers shared by the translation units. |
| `renderer_batch.cpp`    | CPU-side quad staging, radix sorting and draw-call batching. |
| `renderer_gl.cpp`       | OpenGL resource creation and draw submission. |

Companion modules in sibling folders (not in this directory):

- `common/graphics/camera/` — orthographic camera and view-projection matrix.
- `common/graphics/shader/`   — shader program compile/link helper.
- `common/graphics/texture/`  — GPU texture upload and sampling settings.
