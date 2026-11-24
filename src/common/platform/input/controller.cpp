/**
 * @file controller.cpp
 * @brief Gamepad (controller) input state implementation.
 */
#include "controller.hpp"

#include <string.h>

/// @brief Zeroes out all gamepad state.
void ControllerInput::clear()
{
    memset(this, 0, sizeof(*this));
}

/// @brief Copies buttons into previous state.
void ControllerInput::roll()
{
    memcpy(this->buttons_prev, this->buttons,
           sizeof(this->buttons));
}

/// @brief Polls GLFW for the latest gamepad state.
void ControllerInput::poll()
{
    this->connected = glfwJoystickPresent(INPUT_GAMEPAD_JOYSTICK)
                   && glfwJoystickIsGamepad(INPUT_GAMEPAD_JOYSTICK);
    if (!this->connected)
    {
        memset(this->buttons, 0, sizeof(this->buttons));
        memset(this->axes, 0, sizeof(this->axes));
        return;
    }

    GLFWgamepadstate state;
    if (glfwGetGamepadState(INPUT_GAMEPAD_JOYSTICK, &state))
    {
        for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; ++i)
            this->buttons[i] = (state.buttons[i] == GLFW_PRESS);
        for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; ++i)
            this->axes[i] = state.axes[i];
    }
}

/// @brief Returns true while `button` is held down.
/// @param button The gamepad button code.
/// @return True when the button is currently held.
bool ControllerInput::is_gamepad_down(const int button) const
{
    return this->buttons[button];
}

/// @brief Returns true on the frame `button` transitions to held.
/// @param button The gamepad button code.
/// @return True when the button was pressed this frame.
bool ControllerInput::is_gamepad_pressed(const int button) const
{
    return this->buttons[button] && !this->buttons_prev[button];
}

/// @brief Returns true on the frame `button` transitions to released.
/// @param button The gamepad button code.
/// @return True when the button was released this frame.
bool ControllerInput::is_gamepad_released(const int button) const
{
    return !this->buttons[button] && this->buttons_prev[button];
}

/// @brief Returns the axis value in [-1,1] for `axis`.
/// @param axis The gamepad axis code.
/// @return The current axis value.
f32 ControllerInput::gamepad_axis(const int axis) const
{
    return this->axes[axis];
}
