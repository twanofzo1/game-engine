# external/

Third-party dependencies.

| Directory   | Source                                      | Version        |
|-------------|---------------------------------------------|----------------|
| `glfw/`     | https://github.com/glfw/glfw                | 3.3-stable     |
| `imgui/`    | https://github.com/ocornut/imgui            | docking branch |
| `glad/`     | GLAD downloader (glad.dav1d.de)             | 0.1.36, GL 3.3 |
| `stb/`      | https://github.com/nothings/stb (stb_image) | v2.30          |
| `dr_libs/`  | https://github.com/mackron/dr_libs          | dr_wav 0.14.6, dr_mp3 0.7.4 |
| `miniaudio/`| https://github.com/mackron/miniaudio        | v0.11.26       |
| `yaml-cpp/` | https://github.com/jbeder/yaml-cpp          | 0.8.0 (MIT)    |

GLAD's C loader (`glad.c` / `glad/glad.h`) was generated with the GLAD web
downloader (GL 3.3) and is compiled by SCons. GLFW is linked as a shared
library (system `libglfw.so` on Linux, `glfw3.dll` on Windows). `stb_image`
and the dr_libs decoders are single-header libraries compiled from
`src/resources`. `miniaudio` is a single-header library compiled from
`src/audio` (the audio playback manager).
