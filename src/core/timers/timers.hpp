/**
 * @file timers.hpp
 * @brief Declares the timer system: scheduled one-shot and repeating callbacks.
 *
 * A timer system is a small manager that holds scheduled callbacks. The main
 * loop advances it once per frame with `update(manager, dt)`; any timer whose
 * delay has elapsed fires its callback.
 */
#pragma once

#include "core/typedefs.hpp"

namespace Engine {
namespace Timers {

/** One scheduled timer. */
struct Timer {
    f32  remaining;               ///< Seconds until it fires.
    f32  interval;                ///< Repeat interval in seconds.
    bool repeating;               ///< Re-fire every interval.
    bool active;                  ///< Slot is in use.
    void (*callback)(void* user); ///< Called when the timer fires.
    void* user;                   ///< User data passed to the callback.
};

/** Growable set of timers, advanced by update() each frame. */
struct Manager {
    Timer* timers;   ///< Dynamic array of timers.
    u32    capacity; ///< Allocated timer slots.
};

void init(Manager* manager, u32 capacity);
void destroy(Manager* manager);

/**
 * @brief Schedule a callback.
 * @param manager   Timer manager.
 * @param seconds   Delay before the first fire (seconds, >= 0).
 * @param repeating Fire every `seconds` instead of once.
 * @param callback  Function to call when it fires (may be NULL).
 * @param user      User data passed to `callback`.
 * @return Timer handle for cancel(), or 0xFFFFFFFFu on allocation failure.
 */
u32 start(Manager* manager, f32 seconds, bool repeating,
          void (*callback)(void* user), void* user);

/** @brief Cancel the timer identified by `handle`. */
void cancel(Manager* manager, u32 handle);

/** @brief Advance every timer by `dt` and fire the due ones. */
void update(Manager* manager, f32 dt);

} // namespace Timers
} // namespace Engine
