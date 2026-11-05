/**
 * @file timers.cpp
 * @brief Timer system implementation.
 */
#include "core/timers/timers.hpp"

#include <stdlib.h>

namespace Engine {
namespace Timers {

void init(Manager* manager, u32 capacity)
{
    manager->capacity = capacity == 0 ? 8u : capacity;
    manager->timers = (Timer*)calloc(manager->capacity, sizeof(Timer));
}

void destroy(Manager* manager)
{
    free(manager->timers);
    manager->timers = NULL;
    manager->capacity = 0;
}

/** @brief Grow the timer array to hold at least `needed` timers. */
static void grow(Manager* manager, u32 needed)
{
    if (needed <= manager->capacity)
        return;

    u32 next = manager->capacity == 0 ? 8u : manager->capacity * 2u;
    while (next < needed)
        next *= 2u;

    u32 old = manager->capacity;
    /* @warning: raw realloc used for the timer array; growth is rare. */
    manager->timers = (Timer*)realloc(manager->timers, sizeof(Timer) * next);
    for (u32 i = old; i < next; ++i)
        manager->timers[i].active = false;
    manager->capacity = next;
}

u32 start(Manager* manager, f32 seconds, bool repeating,
          void (*callback)(void* user), void* user)
{
    u32 slot = 0xFFFFFFFFu;
    for (u32 i = 0; i < manager->capacity; ++i)
    {
        if (!manager->timers[i].active)
        {
            slot = i;
            break;
        }
    }

    if (slot == 0xFFFFFFFFu)
    {
        u32 old = manager->capacity;
        grow(manager, manager->capacity + 1);
        slot = old; /* first newly added slot */
    }

    Timer* timer = &manager->timers[slot];
    timer->remaining = seconds < 0.0f ? 0.0f : seconds;
    timer->interval  = timer->remaining;
    timer->repeating = repeating;
    timer->callback  = callback;
    timer->user      = user;
    timer->active    = true;
    return slot;
}

void cancel(Manager* manager, u32 handle)
{
    if (handle < manager->capacity)
        manager->timers[handle].active = false;
}

void update(Manager* manager, f32 dt)
{
    for (u32 i = 0; i < manager->capacity; ++i)
    {
        Timer* timer = &manager->timers[i];
        if (!timer->active)
            continue;

        timer->remaining -= dt;
        if (timer->remaining > 0.0f)
            continue;

        if (timer->repeating && timer->interval > 0.0f)
        {
            /* Keep the overshoot so the cadence stays accurate. */
            timer->remaining += timer->interval;
            if (timer->callback)
                timer->callback(timer->user);
        }
        else
        {
            timer->active = false;
            if (timer->callback)
                timer->callback(timer->user);
        }
    }
}

} // namespace Timers
} // namespace Engine
