/**
 * @file input.hpp
 * @brief Unified keyboard + mouse + gamepad input state.
 *
 * One Input struct aggregates the state of all three devices. Call
 * begin_frame() at the start of every frame (before glfwPollEvents()) so the
 * previous state is rolled and per-frame deltas are reset; the query helpers
 * then report held/pressed/released state and axis/delta values.
 */
#pragma once

#include "typedefs.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "controller.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/** Maximum number of keys that can be bound to a single action. */
#define INPUT_ACTION_KEYS_MAX 4

/** A named action with the keys bound to it. */
struct Action {
    const char* name;                        ///< Action name.
    int         keys[INPUT_ACTION_KEYS_MAX]; ///< Bound keys; -1 = unused.
};

/** Aggregated keyboard + mouse + gamepad state. */
struct Input {
    GLFWwindow* window; ///< Window this input is bound to.

    KeyboardInput  keyboard;   ///< Keyboard state.
    MouseInput     mouse;      ///< Mouse state.
    ControllerInput controller; ///< Gamepad state.

    /* Actions. */
    Action* actions;      ///< Action table (caller-owned).
    u32     action_count; ///< Number of entries in `actions`.

    static void init(Input* input, GLFWwindow* window);
    static void shutdown(Input* input);
    static void begin_frame(Input* input);


    static void set_actions(Input* input, Action* actions, u32 count);
    static const Action* find_action(const Input* input, const char* name);
    static bool bind_key(Input* input, const char* name, int key);

    static bool is_action_down(const Input* input, const char* name);
    static bool is_action_pressed(const Input* input, const char* name);
    static bool is_action_released(const Input* input, const char* name);
};