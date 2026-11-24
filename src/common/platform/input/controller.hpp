/**
 * @file controller.hpp
 * @brief Gamepad (controller) input state.
 *
 * Tracks connection status, button state and axis values for a single
 * gamepad. The state is polled from GLFW by Input::begin_frame().
 */
#pragma once

#include "common/core/typedefs.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/** Gamepad slot polled by the input module. Macros cannot be namespaced. */
#define INPUT_GAMEPAD_JOYSTICK GLFW_JOYSTICK_1

/** Gamepad state: connection, buttons and axes. */
struct ControllerInput {
    bool connected;                                    ///< True when the gamepad is present.
    bool buttons[GLFW_GAMEPAD_BUTTON_LAST + 1];        ///< Current gamepad button state.
    bool buttons_prev[GLFW_GAMEPAD_BUTTON_LAST + 1];   ///< Previous gamepad button state.
    f32  axes[GLFW_GAMEPAD_AXIS_LAST + 1];             ///< Axis values in [-1,1].

    void clear();
    void roll();
    void poll();
    bool is_gamepad_down(int button) const;
    bool is_gamepad_pressed(int button) const;
    bool is_gamepad_released(int button) const;
    f32 gamepad_axis(int axis) const;
};
