/**
 * @file input.cpp
 * @brief Unified keyboard + mouse + gamepad input implementation.
 */
#include "input.hpp"

#include <string.h>

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
    if (!input)
        return;

    if (action == GLFW_PRESS)
        KeyboardInput::on_key(&input->keyboard, key, true);
    else if (action == GLFW_RELEASE)
        KeyboardInput::on_key(&input->keyboard, key, false);
    /* GLFW_REPEAT keeps the key held; no state change needed. */
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void)mods;

    Input* input = from_window(window);
    if (!input)
        return;

    MouseInput::on_button(&input->mouse, button, action == GLFW_PRESS);
}

static void cursor_callback(GLFWwindow* window, f64 x, f64 y)
{
    Input* input = from_window(window);
    if (!input)
        return;

    MouseInput::on_cursor(&input->mouse, x, y);
}

static void scroll_callback(GLFWwindow* window, f64 xoff, f64 yoff)
{
    Input* input = from_window(window);
    if (!input)
        return;

    MouseInput::on_scroll(&input->mouse, xoff, yoff);
}

/** @brief Bind the input to `window` and register GLFW callbacks. */
void Input::init(Input* input, GLFWwindow* window)
{
    memset(input, 0, sizeof(*input));
    input->window = window;

    glfwSetWindowUserPointer(window, input);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwGetCursorPos(window, &input->mouse.mouse_x, &input->mouse.mouse_y);
}

/** @brief Unregister callbacks and zero out all state. */
void Input::shutdown(Input* input)
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

/** @brief Call at the start of each frame, before glfwPollEvents(). */
void Input::begin_frame(Input* input)
{
    KeyboardInput::roll(&input->keyboard);
    MouseInput::roll(&input->mouse);
    ControllerInput::roll(&input->controller);
    ControllerInput::poll(&input->controller);
}

/** @brief Install the action table; the array must outlive the input. */
void Input::set_actions(Input* input, Action* actions, u32 count)
{
    input->actions = actions;
    input->action_count = count;
}

/** @brief Find an action by name, or NULL. */
const Action* Input::find_action(const Input* input, const char* name)
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

/** @brief Append `key` to the named action. Returns false if not found or full. */
bool Input::bind_key(Input* input, const char* name, int key)
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

/** @brief True when any key bound to `name` is held down. */
bool Input::is_action_down(const Input* input, const char* name)
{
    const Action* action = Input::find_action(input, name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        int key = action->keys[i];
        if (key >= 0 && KeyboardInput::is_key_down(&input->keyboard, key))
            return true;
    }
    return false;
}

/** @brief True on the frame any key bound to `name` transitions to held. */
bool Input::is_action_pressed(const Input* input, const char* name)
{
    const Action* action = Input::find_action(input, name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        int key = action->keys[i];
        if (key >= 0 && KeyboardInput::is_key_pressed(&input->keyboard, key))
            return true;
    }
    return false;
}

/** @brief True on the frame any key bound to `name` transitions to released. */
bool Input::is_action_released(const Input* input, const char* name)
{
    const Action* action = Input::find_action(input, name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        int key = action->keys[i];
        if (key >= 0 && KeyboardInput::is_key_released(&input->keyboard, key))
            return true;
    }
    return false;
}


