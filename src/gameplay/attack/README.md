# attack/

Attack definitions: phased timing (windup/active/recovery) plus event and
movement keyframe timelines. Events are fired through an `Event::Bus`
(subscriber pattern) as playback crosses their timestamps.

| File          | Purpose |
|---------------|---------|
| `attack.hpp`  | `Attack` type, keyframe helpers and `Playback` state. |
| `attack.cpp`  | Keyframe storage, lookups and event-firing implementation. |
