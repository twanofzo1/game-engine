# gameplay/

Game simulation logic: animation playback, attack definitions and 2D
collision shapes. These modules hold data and logic but no rendering or
platform code.

| Directory   | Purpose |
|-------------|---------|
| `animation/` | Sprite animation playback (keyframes). |
| `attack/`    | Attack data: phased timing plus hitbox/event/movement keyframes. |
| `collision/` | 2D collision shapes and overlap testing. |
