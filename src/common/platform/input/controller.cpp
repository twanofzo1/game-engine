/**
 * @file controller.cpp
 * @brief Gamepad (controller) input state implementation.
 */
#include "controller.hpp"

#include <string.h>

/** @brief Zero out all gamepad state. */
void ControllerInput::clear(ControllerInput* controller)
{
    memset(controller, 0, sizeof(*controller));
}

/** @brief Copy buttons into previous state. */
void ControllerInput::roll(ControllerInput* controller)
{
    memcpy(controller->buttons_prev, controller->buttons,
           sizeof(controller->buttons));
}

/** @brief Poll GLFW for the latest gamepad state. */
void ControllerInput::poll(ControllerInput* controller)
{
    controller->connected = glfwJoystickPresent(INPUT_GAMEPAD_JOYSTICK)
                         && glfwJoystickIsGamepad(INPUT_GAMEPAD_JOYSTICK);
    if (!controller->connected)
    {
        memset(controller->buttons, 0, sizeof(controller->buttons));
        memset(controller->axes, 0, sizeof(controller->axes));
        return;
    }

    GLFWgamepadstate state;
    if (glfwGetGamepadState(INPUT_GAMEPAD_JOYSTICK, &state))
    {
        for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; ++i)
            controller->buttons[i] = (state.buttons[i] == GLFW_PRESS);
        for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; ++i)
            controller->axes[i] = state.axes[i];
    }
}

/** @brief True while `button` is held down. */
bool ControllerInput::is_gamepad_down(const ControllerInput* controller, int button)
{
    return controller->buttons[button];
}

/** @brief True on the frame `button` transitions to held. */
bool ControllerInput::is_gamepad_pressed(const ControllerInput* controller, int button)
{
    return controller->buttons[button] && !controller->buttons_prev[button];
}

/** @brief True on the frame `button` transitions to released. */
bool ControllerInput::is_gamepad_released(const ControllerInput* controller, int button)
{
    return !controller->buttons[button] && controller->buttons_prev[button];
}

/** @brief Axis value in [-1,1] for `axis`. */
f32 ControllerInput::gamepad_axis(const ControllerInput* controller, int axis)
{
    return controller->axes[axis];
}
