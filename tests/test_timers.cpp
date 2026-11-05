/**
 * @file test_timers.cpp
 * @brief Tests for the timer system (one-shot, repeating, cancel, growth).
 */
#include "framework.hpp"

#include "core/timers/timers.hpp"

#include <stddef.h>

/** @brief Callback that increments the int pointed to by `user`. */
static void bump(void* user)
{
    int* n = (int*)user;
    ++*n;
}

void test_timers(void)
{
    Engine::Timers::Manager manager;
    Engine::Timers::init(&manager, 4);

    /* One-shot fires exactly once, after its delay has elapsed. */
    int hits = 0;
    Engine::Timers::start(&manager, 1.0f, false, bump, &hits);
    Engine::Timers::update(&manager, 0.4f);
    TEST_CHECK(hits == 0);
    Engine::Timers::update(&manager, 0.6f);
    TEST_CHECK(hits == 1);
    Engine::Timers::update(&manager, 5.0f);
    TEST_CHECK(hits == 1);

    /* Repeating fires once per interval. */
    int reps = 0;
    Engine::Timers::start(&manager, 1.0f, true, bump, &reps);
    Engine::Timers::update(&manager, 1.0f);
    TEST_CHECK(reps == 1);
    Engine::Timers::update(&manager, 1.0f);
    TEST_CHECK(reps == 2);
    Engine::Timers::update(&manager, 0.25f);
    TEST_CHECK(reps == 2);

    /* Cancelling prevents the callback. */
    int cancelled = 0;
    u32 handle = Engine::Timers::start(&manager, 0.5f, false, bump, &cancelled);
    Engine::Timers::cancel(&manager, handle);
    Engine::Timers::update(&manager, 1.0f);
    TEST_CHECK(cancelled == 0);

    /* A zero-delay timer fires on the next update. */
    int zero = 0;
    Engine::Timers::start(&manager, 0.0f, false, bump, &zero);
    Engine::Timers::update(&manager, 0.0f);
    TEST_CHECK(zero == 1);

    /* Starting more timers than the initial capacity grows the array. */
    int total = 0;
    for (int i = 0; i < 10; ++i)
        Engine::Timers::start(&manager, 1.0f, false, bump, &total);
    Engine::Timers::update(&manager, 1.0f);
    TEST_CHECK(total == 10);

    Engine::Timers::destroy(&manager);
    TEST_CHECK(manager.timers == NULL);
    TEST_CHECK(manager.capacity == 0);
}
