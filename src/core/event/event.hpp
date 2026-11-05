/**
 * @file event.hpp
 * @brief Minimal publish/subscribe event bus.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Event {

/** @brief Event handler signature: (user data, event name, payload). */
typedef void (*Handler)(void* user, const char* event, void* payload);

/** Interned event identifier. 0 = "no event" / "match every event". */
typedef u32 EventId;

/** One subscription: a handler registered for an event id (0 = all). */
struct Subscription {
    EventId event_id; ///< Event to listen for; 0 = every event.
    Handler handler;
    void*   user;
};

/** A growable list of subscriptions. */
struct Bus {
    Subscription* subs;
    u32 count;
    u32 capacity;
};

void init(Bus* bus);
void destroy(Bus* bus);

/** @brief Register a handler for `event` (NULL = all events). */
void subscribe(Bus* bus, const char* event, Handler handler, void* user);

/** @brief Notify every handler listening to `event`, passing `payload`. */
void emit(const Bus* bus, const char* event, void* payload);

/** @brief Notify every handler listening to `event_id`, passing `payload`.
 *  @warning `event_id` must come from `intern`; unknown ids dispatch nothing. */
void emit_id(const Bus* bus, EventId event_id, void* payload);

/** @brief Intern an event name into a stable id (assigns one on first use).
 *         NULL or "" maps to 0. */
EventId intern(const char* name);

/** @brief Canonical name for an interned id, or NULL for 0. */
const char* name_of(EventId id);

/** Maps a symbolic handler name (used in data files) to a compiled handler. */
struct Binding {
    const char* name;    ///< Symbolic name used in data files.
    Handler     handler; ///< Compiled function to call.
};

/**
 * @brief Find a handler by name in `bindings`.
 * @return The matching handler, or NULL when not found.
 */
Handler lookup(const Binding* bindings, u32 count, const char* name);

} // namespace Event
} // namespace Engine
