/**
 * @file mouse.hpp
 * @brief Mouse input state.
 *
 * Tracks button state, cursor position and per-frame cursor/scroll deltas.
 * The state is filled by GLFW mouse callbacks and rolled/reset by
 * Input::begin_frame().
 */
#pragma once

#include "typedefs.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/** Mouse state: buttons, cursor position and per-frame deltas. */
struct MouseInput {
    bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1];      ///< Current button state.
    bool mouse_buttons_prev[GLFW_MOUSE_BUTTON_LAST + 1]; ///< Previous button state.
    f64  mouse_x;  ///< Cursor X in window coordinates.
    f64  mouse_y;  ///< Cursor Y in window coordinates.
    f64  mouse_dx; ///< Cursor X delta accumulated this frame.
    f64  mouse_dy; ///< Cursor Y delta accumulated this frame.
    f64  scroll_x; ///< Scroll X delta accumulated this frame.
    f64  scroll_y; ///< Scroll Y delta accumulated this frame.

    static void clear(MouseInput* mouse);
    static void roll(MouseInput* mouse);
    static void on_button(MouseInput* mouse, int button, bool pressed);
    static void on_cursor(MouseInput* mouse, f64 x, f64 y);
    static void on_scroll(MouseInput* mouse, f64 xoff, f64 yoff);
    static bool is_mouse_down(const MouseInput* mouse, int button);
    static bool is_mouse_pressed(const MouseInput* mouse, int button);
    static bool is_mouse_released(const MouseInput* mouse, int button);
};
