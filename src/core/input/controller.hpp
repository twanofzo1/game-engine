/**
 * @file controller.hpp
 * @brief Gamepad (controller) input state.
 *
 * Tracks connection status, button state and axis values for a single
 * gamepad. The state is polled from GLFW by Input::begin_frame().
 */
#pragma once

#include "typedefs.hpp"

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

    static void clear(ControllerInput* controller);
    static void roll(ControllerInput* controller);
    static void poll(ControllerInput* controller);
    static bool is_gamepad_down(const ControllerInput* controller, int button);
    static bool is_gamepad_pressed(const ControllerInput* controller, int button);
    static bool is_gamepad_released(const ControllerInput* controller, int button);
    static f32 gamepad_axis(const ControllerInput* controller, int axis);
};
