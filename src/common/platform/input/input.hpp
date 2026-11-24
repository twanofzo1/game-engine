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

#include "common/core/typedefs.hpp"
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

    void init(GLFWwindow* window);
    void shutdown();
    void begin_frame();

    void set_actions(Action* actions, u32 count);
    const Action* find_action(const char* name) const;
    bool bind_key(const char* name, int key);

    bool is_action_down(const char* name) const;
    bool is_action_pressed(const char* name) const;
    bool is_action_released(const char* name) const;
};