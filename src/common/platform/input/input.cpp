/**
 * @file input.cpp
 * @brief Unified keyboard + mouse + gamepad input implementation.
 */
#include "input.hpp"

#include <string.h>

/// @brief Fetches the Input bound to `window` via its user pointer.
/// @param window The GLFW window.
/// @return The bound Input, or nullptr when none is set.
static Input* from_window(GLFWwindow* const window)
{
    return (Input*)glfwGetWindowUserPointer(window);
}

static void key_callback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods)
{
    (void)scancode;
    (void)mods;

    Input* const input = from_window(window);
    if (!input)
        return;

    if (action == GLFW_PRESS)
        input->keyboard.on_key(key, true);
    else if (action == GLFW_RELEASE)
        input->keyboard.on_key(key, false);
    /* GLFW_REPEAT keeps the key held; no state change needed. */
}

static void mouse_button_callback(GLFWwindow* const window, const int button, const int action, const int mods)
{
    (void)mods;

    Input* const input = from_window(window);
    if (!input)
        return;

    input->mouse.on_button(button, action == GLFW_PRESS);
}

static void cursor_callback(GLFWwindow* const window, const f64 x, const f64 y)
{
    Input* const input = from_window(window);
    if (!input)
        return;

    input->mouse.on_cursor(x, y);
}

static void scroll_callback(GLFWwindow* const window, const f64 xoff, const f64 yoff)
{
    Input* const input = from_window(window);
    if (!input)
        return;

    input->mouse.on_scroll(xoff, yoff);
}

/// @brief Binds this input to `window` and registers GLFW callbacks.
/// @param window The GLFW window to bind.
void Input::init(GLFWwindow* const window)
{
    memset(this, 0, sizeof(*this));
    this->window = window;

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwGetCursorPos(window, &this->mouse.mouse_x, &this->mouse.mouse_y);
}

/// @brief Unregisters callbacks and zeroes out all state.
void Input::shutdown()
{
    if (this->window)
    {
        glfwSetKeyCallback(this->window, NULL);
        glfwSetMouseButtonCallback(this->window, NULL);
        glfwSetCursorPosCallback(this->window, NULL);
        glfwSetScrollCallback(this->window, NULL);
        glfwSetWindowUserPointer(this->window, NULL);
    }
    memset(this, 0, sizeof(*this));
}

/// @brief Rolls device state forward at the start of each frame.
void Input::begin_frame()
{
    this->keyboard.roll();
    this->mouse.roll();
    this->controller.roll();
    this->controller.poll();
}

/// @brief Installs the action table; the array must outlive this input.
/// @param actions The action table.
/// @param count The number of entries in `actions`.
void Input::set_actions(Action* const actions, const u32 count)
{
    this->actions = actions;
    this->action_count = count;
}

/// @brief Finds an action by name.
/// @param name The action name.
/// @return The matching action, or nullptr when not found.
const Action* Input::find_action(const char* const name) const
{
    if (!name)
        return nullptr;

    for (u32 i = 0; i < this->action_count; ++i)
    {
        if (strcmp(this->actions[i].name, name) == 0)
            return &this->actions[i];
    }
    return nullptr;
}

/// @brief Appends `key` to the named action.
/// @param name The action name.
/// @param key The key code to bind.
/// @return False when the action is not found or has no free slot.
bool Input::bind_key(const char* const name, const int key)
{
    if (!name)
        return false;

    for (u32 i = 0; i < this->action_count; ++i)
    {
        Action* const action = &this->actions[i];
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

/// @brief Returns true when any key bound to `name` is held down.
/// @param name The action name.
/// @return True when any bound key is currently held.
bool Input::is_action_down(const char* const name) const
{
    const Action* const action = this->find_action(name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        const int key = action->keys[i];
        if (key >= 0 && this->keyboard.is_key_down(key))
            return true;
    }
    return false;
}

/// @brief Returns true on the frame any key bound to `name` transitions to held.
/// @param name The action name.
/// @return True when any bound key was pressed this frame.
bool Input::is_action_pressed(const char* const name) const
{
    const Action* const action = this->find_action(name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        const int key = action->keys[i];
        if (key >= 0 && this->keyboard.is_key_pressed(key))
            return true;
    }
    return false;
}

/// @brief Returns true on the frame any key bound to `name` transitions to released.
/// @param name The action name.
/// @return True when any bound key was released this frame.
bool Input::is_action_released(const char* const name) const
{
    const Action* const action = this->find_action(name);
    if (!action)
        return false;

    for (int i = 0; i < INPUT_ACTION_KEYS_MAX; ++i)
    {
        const int key = action->keys[i];
        if (key >= 0 && this->keyboard.is_key_released(key))
            return true;
    }
    return false;
}


