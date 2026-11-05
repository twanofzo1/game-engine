# tests/

Headless test suite for the pure-logic engine modules. It does not link
GLFW, GL, a window or an audio device, so it runs on machines without a
display.

Build and run with:

    scons test              # build and run the tests (uses build=release)
    scons build=debug test  # build and run the debug test binary

The test binary is written to `build/<build>/tests` and can also be run
directly.

| File                | Purpose |
|---------------------|---------|
| `framework.hpp`     | Harness: registration, assertions (`TEST_CHECK*`) and a runner. |
| `framework.cpp`     | Harness implementation: registry, runner and check accounting. |
| `run_tests.cpp`     | Entry point: registers every test and runs them. |
| `test_types.cpp`    | Vec2/Vec2i, Rect/Recti, Color, Transform2D/Transform2Di. |
| `test_memory_pool.cpp` | Fixed-size block allocator. |
| `test_ecs.cpp`      | World lifecycle, component add/get/remove, slot reuse. |
| `test_camera.cpp`   | Orthographic view-projection matrix. |
| `test_resource.cpp` | File reading and the resource cache. |
| `test_image.cpp`    | stb_image decoding of a generated BMP. |
| `test_audio.cpp`    | dr_wav decoding of a generated WAV plus failure paths. |
