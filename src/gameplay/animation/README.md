# animation/

Timeline animation: a named sequence of (time, frame) keyframes played back on
a sprite (referenced by index into the sprite list).

| File            | Purpose |
|-----------------|---------|
| `animation.hpp` | `Animation` type and playback helpers (`add_frame`/`duration`/`frame_at`). |
| `animation.cpp` | Keyframe storage and frame-lookup implementation. |
