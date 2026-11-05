# event/

Minimal publish/subscribe event bus. Event names are interned to integer ids
(`Event::intern` / `EventId`), so subscriptions and dispatch compare integers
instead of strings. Handlers subscribe to an event name (`NULL` = every event)
and are notified via `emit` (string) or `emit_id` (pre-interned id, zero string
work in the hot path), each carrying an optional `void* payload`. A `Binding`
table plus `lookup` maps symbolic handler names (as written in YAML) to
compiled functions.

| File         | Purpose |
|--------------|---------|
| `event.hpp`  | `Bus` / `Subscription` / `Binding` types, `EventId` and declarations. |
| `event.cpp`  | Intern table, subscription list, id dispatch and name lookup. |
