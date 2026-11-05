# timers/

Timer system: scheduled one-shot and repeating callbacks.

| File           | Purpose |
|----------------|---------|
| `timers.hpp`   | `Engine::Timers` API: init/destroy, start/cancel, per-frame update. |
| `timers.cpp`   | Timer manager implementation (growable slot array). |

Usage: call `Timers::update(&manager, dt)` once per frame. Timers fire their
callback when their delay elapses; repeating timers re-arm for the next
interval.

not a component cause it uses a user pointer callback