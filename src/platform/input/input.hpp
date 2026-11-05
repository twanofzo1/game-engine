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

#include "core/typedefs.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/** Gamepad slot polled by the input module. Macros cannot be namespaced. */
#define INPUT_GAMEPAD_JOYSTICK GLFW_JOYSTICK_1

/** Maximum number of keys that can be bound to a single action. */
#define INPUT_ACTION_KEYS_MAX 4

namespace Engine {
namespace Input {

/** A named action with the keys bound to it. */
struct Action {
    const char* name;                        ///< Action name.
    int         keys[INPUT_ACTION_KEYS_MAX]; ///< Bound keys; -1 = unused.
};

/** Aggregated keyboard + mouse + gamepad state. */
struct Input {
    GLFWwindow* window; ///< Window this input is bound to.

    /* Keyboard. */
    bool keys[GLFW_KEY_LAST + 1];      ///< Current key state.
    bool keys_prev[GLFW_KEY_LAST + 1]; ///< Previous key state.

    /* Mouse. */
    bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1];      ///< Current button state.
    bool mouse_buttons_prev[GLFW_MOUSE_BUTTON_LAST + 1]; ///< Previous button state.
    f64  mouse_x;  ///< Cursor X in window coordinates.
    f64  mouse_y;  ///< Cursor Y in window coordinates.
    f64  mouse_dx; ///< Cursor X delta accumulated this frame.
    f64  mouse_dy; ///< Cursor Y delta accumulated this frame.
    f64  scroll_x; ///< Scroll X delta accumulated this frame.
    f64  scroll_y; ///< Scroll Y delta accumulated this frame.

    /* Gamepad. */
    bool gamepad_connected;                                  ///< True when the gamepad is present.
    bool gamepad_buttons[GLFW_GAMEPAD_BUTTON_LAST + 1];      ///< Current gamepad button state.
    bool gamepad_buttons_prev[GLFW_GAMEPAD_BUTTON_LAST + 1]; ///< Previous gamepad button state.
    f32  gamepad_axes[GLFW_GAMEPAD_AXIS_LAST + 1];           ///< Axis values in [-1,1].

    /* Actions. */
    Action* actions;      ///< Action table (caller-owned).
    u32     action_count; ///< Number of entries in `actions`.
};

void init(Input* input, GLFWwindow* window);
void shutdown(Input* input);

/** @brief Call at the start of each frame, before glfwPollEvents(). */
void begin_frame(Input* input);

/* --- Keyboard queries. --- */
static inline bool is_key_down(const Input* in, int key)
{ return in->keys[key]; }

static inline bool is_key_pressed(const Input* in, int key)
{ return in->keys[key] && !in->keys_prev[key]; }

static inline bool is_key_released(const Input* in, int key)
{ return !in->keys[key] && in->keys_prev[key]; }

/* --- Mouse queries. --- */
static inline bool is_mouse_down(const Input* in, int button)
{ return in->mouse_buttons[button]; }

static inline bool is_mouse_pressed(const Input* in, int button)
{ return in->mouse_buttons[button] && !in->mouse_buttons_prev[button]; }

static inline bool is_mouse_released(const Input* in, int button)
{ return !in->mouse_buttons[button] && in->mouse_buttons_prev[button]; }

static inline f64 mouse_x(const Input* in) { return in->mouse_x; }
static inline f64 mouse_y(const Input* in) { return in->mouse_y; }
static inline f64 mouse_dx(const Input* in) { return in->mouse_dx; }
static inline f64 mouse_dy(const Input* in) { return in->mouse_dy; }
static inline f64 scroll_x(const Input* in) { return in->scroll_x; }
static inline f64 scroll_y(const Input* in) { return in->scroll_y; }

/* --- Gamepad queries. --- */
static inline bool gamepad_connected(const Input* in)
{ return in->gamepad_connected; }

static inline bool is_gamepad_down(const Input* in, int button)
{ return in->gamepad_buttons[button]; }

static inline bool is_gamepad_pressed(const Input* in, int button)
{ return in->gamepad_buttons[button] && !in->gamepad_buttons_prev[button]; }

static inline bool is_gamepad_released(const Input* in, int button)
{ return !in->gamepad_buttons[button] && in->gamepad_buttons_prev[button]; }

static inline f32 gamepad_axis(const Input* in, int axis)
{ return in->gamepad_axes[axis]; }

/* --- Action mapping. --- */

/** @brief Install the action table; the array must outlive the input. */
void set_actions(Input* input, Action* actions, u32 count);

/** @brief Find an action by name, or NULL. */
const Action* find_action(const Input* input, const char* name);

/** @brief Append `key` to the named action. Returns false if not found or full. */
bool bind_key(Input* input, const char* name, int key);

/* --- Action queries (true when any bound key matches). --- */
bool is_action_down(const Input* input, const char* name);
bool is_action_pressed(const Input* input, const char* name);
bool is_action_released(const Input* input, const char* name);

} // namespace Input
} // namespace Engine
