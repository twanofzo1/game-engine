/**
 * @file keyboard.hpp
 * @brief Keyboard input state.
 *
 * Tracks the current and previous frame's key state so callers can query
 * held, pressed and released keys. The state is filled by GLFW key callbacks
 * and rolled forward by Input::begin_frame().
 */
#pragma once

#include "common/core/typedefs.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/** Keyboard state: current and previous frame key arrays. */
struct KeyboardInput {
    bool keys[GLFW_KEY_LAST + 1];      ///< Current key state.
    bool keys_prev[GLFW_KEY_LAST + 1]; ///< Previous key state.

    static void clear(KeyboardInput* kb);
    static void roll(KeyboardInput* kb);
    static void on_key(KeyboardInput* kb, int key, bool pressed);
    static bool is_key_down(const KeyboardInput* kb, int key);
    static bool is_key_pressed(const KeyboardInput* kb, int key);
    static bool is_key_released(const KeyboardInput* kb, int key);
};
