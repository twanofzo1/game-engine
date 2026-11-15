# 2D Game Engine

A small, C-like, ECS-based 2D game engine on OpenGL 3.3. It follows the
conventions in `rules.md`: C-style code, no classes/inheritance, composition
over inheritance, a custom memory pool and doxygen comments.

## Stack

- **GLFW 3.3** — window, input and OpenGL context (linked as a shared library)
- **GLAD** — OpenGL loader (downloaded via the GLAD generator into `external/glad`)
- **Dear ImGui (docking branch)** — immediate-mode debug UI (compiled from `external/imgui`)

## Build

```sh
scons build=debug     # -> bin/debug/{editor,runtime}
scons build=release   # -> bin/release/{editor,runtime}
```

`release` is the default build type. The build produces two executables:
- `editor` — the windowed YAML editor.
- `runtime` — the game runtime (loads YAML, runs the game).

## Run

```sh
./bin/release/editor
./bin/release/runtime path/to/scene.yaml
```

## Tests

The headless test suite covers the pure-logic modules (types, ECS, memory
pool, camera, resource cache, image/audio decoding) and runs without a
display or audio device:

```sh
scons test               # build + run tests (release config)
scons build=debug test   # build + run tests (debug config)
```

The test binary is `build/<build>/tests`; see `tests/README.md` for details.

## Directories

| Directory    | Purpose |
|--------------|---------|
| `src/`       | Engine and demo application code |
| `editor/`    | Standalone YAML editor executable (work in progress) |
| `external/`  | Third-party sources (GLFW, ImGui, GLAD) |
| `tests/`     | Headless test suite for the pure-logic modules |

See `rules.md` for coding conventions and `src/README.md` for the engine layout.
