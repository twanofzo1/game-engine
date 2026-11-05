# resources/

Asset management: caches files in memory by path, plus decoders for common
image (PNG, JPG, BMP, GIF, TGA, HDR) and audio (WAV, MP3) formats.

| File           | Purpose |
|----------------|---------|
| `resource.hpp` | Resource cache API (`Engine::Resource`): load/unload/find by path. |
| `resource.cpp` | Resource cache and whole-file reader implementation. |
| `image.hpp`    | `Engine::Image` struct and image loader declarations. |
| `image.cpp`    | Image decoding via stb_image (PNG/JPG/BMP/GIF/TGA/HDR...). |
| `audio.hpp`    | `Engine::Audio` struct and audio loader declarations. |
| `audio.cpp`    | WAV + MP3 decoding via dr_wav/dr_mp3. |
