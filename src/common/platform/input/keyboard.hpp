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

    void clear();
    void roll();
    void on_key(int key, bool pressed);
    bool is_key_down(int key) const;
    bool is_key_pressed(int key) const;
    bool is_key_released(int key) const;
};
