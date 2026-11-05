/**
 * @file input.cpp
 * @brief Unified keyboard + mouse + gamepad input implementation.
 */
#include "platform/input/input.hpp"

#include <string.h>

namespace Engine {
namespace Input {

/** @brief Fetch the Input bound to `window` via its user pointer. */
static Input* from_window(GLFWwindow* window)
{
    return (Input*)glfwGetWindowUserPointer(window);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    Input* input = from_window(window);
    if (!input || key < 0 || key > GLFW_KEY_LAST)
        return;

    if (action == GLFW_PRESS)
        input->keys[key] = true;
    else if (action == GLFW_RELEASE)
        input->keys[key] = false;
    /* GLFW_REPEAT keeps the key held; no state change needed. */
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void)mods;

    Input* input = from_window(window);
    if (!input || button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return;

    input->mouse_buttons[button] = (action == GLFW_PRESS);
}

static void cursor_callback(GLFWwindow* window, f64 x, f64 y)
{
    Input* input = from_window(window);
    if (!input)
        return;

    input->mouse_dx += x - input->mouse_x;
    input->mouse_dy += y - input->mouse_y;
    input->mouse_x = x;
    input->mouse_y = y;
}

static void scroll_callback(GLFWwindow* window, f64 xoff, f64 yoff)
{
    Input* input = from_window(window);
    if (!input)
        return;

    input->scroll_x += xoff;
    input->scroll_y += yoff;
}

void init(Input* input, GLFWwindow* window)
{
    memset(input, 0, sizeof(*input));
    input->window = window;

    glfwSetWindowUserPointer(window, input);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwGetCursorPos(window, &input->mouse_x, &input->mouse_y);
}

void shutdown(Input* input)
{
    if (input->window)
    {
        glfwSetKeyCallback(input->window, NULL);
        glfwSetMouseButtonCallback(input->window, NULL);
        glfwSetCursorPosCallback(input->window, NULL);
        glfwSetScrollCallback(input->window, NULL);
        glfwSetWindowUserPointer(input->window, NULL);
    }
    memset(input, 0, sizeof(*input));
}

void begin_frame(Input* input)
{
    memcpy(input->keys_prev, input->keys, sizeof(input->keys));
    memcpy(input->mouse_buttons_prev, input->mouse_buttons, sizeof(input->mouse_buttons));
    memcpy(input->gamepad_buttons_prev, input->gamepad_buttons, sizeof(input->gamepad_buttons));

    input->mouse_dx = 0.0;
    input->mouse_dy = 0.0;
    input->scroll_x = 0.0;
    input->scroll_y = 0.0;

    input->gamepad_connected = glfwJoystickPresent(INPUT_GAMEPAD_JOYSTICK)
                            && glfwJoystickIsGamepad(INPUT_GAMEPAD_JOYSTICK);
    if (!input->gamepad_connected)
    {
        memset(input->gamepad_buttons, 0, sizeof(input->gamepad_buttons));
        memset(input->gamepad_axes, 0, sizeof(input->gamepad_axes));
        return;
    }

    GLFWgamepadstate state;
    if (glfwGetGamepadState(INPUT_GAMEPAD_JOYSTICK, &state))
    {
        for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; ++i)
            input->gamepad_buttons[i] = (state.buttons[i] == GLFW_PRESS);
        for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; ++i)
            input->gamepad_axes[i] = state.axes[i];
    }
}

void set_actions(Input* input, Action* actions, u32 count)
{
    input->actions = actions;
    input->action_count = count;
}

const Action* find_action(const Input* input, const char* name)
{
    if (!input || !name)
        return NULL;

    for (u32 i = 0; i < input->action_count; ++i)
    {
        if (strcmp(input->actions[i].name, name) == 0)
            return &input->actions[i];
    }
    return NULL;
}

bool bind_key(Input* input, const char* name, int key)
{
    if (!input || !name)
        return false;

    for (u32 i = 0; i < input->action_count; ++i)
    {
        Action* action = &input->actions[i];
        if (strcmp(action->name, name) != 0)
            continue;

        for (int k = 0; k < INPUT_ACTION_KEYS_MAX; ++k)
        {
            if (action->keys[k] < 0)
            {
                action->keys[k] = key;
                return true;
            }
        }
        return false; /* no free binding slot */
    }
    return false;
}

bool is_action_down(const Input* input, const char* name)
{
    const Action* action = find_action(input, name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        int key = action->keys[i];
        if (key >= 0 && is_key_down(input, key))
            return true;
    }
    return false;
}

bool is_action_pressed(const Input* input, const char* name)
{
    const Action* action = find_action(input, name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        int key = action->keys[i];
        if (key >= 0 && is_key_pressed(input, key))
            return true;
    }
    return false;
}

bool is_action_released(const Input* input, const char* name)
{
    const Action* action = find_action(input, name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        int key = action->keys[i];
        if (key >= 0 && is_key_released(input, key))
            return true;
    }
    return false;
}

} // namespace Input
} // namespace Engine
