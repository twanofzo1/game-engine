/**
 * @file window.hpp
 * @brief Declares the GLFW window wrapper and its creation options.
 */
#pragma once

#include "core/typedefs.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine {
namespace Window {

/** Maximum length of the window title, including the null terminator. */
#define WINDOW_TITLE_MAX 64

/** Options used when creating a window. */
struct Options {
    i32  width;       ///< Initial window width in pixels.
    i32  height;      ///< Initial window height in pixels.
    i32  position_x;  ///< Initial X position (-1 = let the OS decide).
    i32  position_y;  ///< Initial Y position (-1 = let the OS decide).
    bool vsync;       ///< Enable vertical sync.
    bool resizable;   ///< Allow the window to be resized.
    bool borderless;  ///< Hide the title bar and borders.
    i32  screen_mode; ///< 0 = windowed, 1 = fullscreen, 2 = borderless fullscreen.
    char title[WINDOW_TITLE_MAX]; ///< Window title.
};

/** @brief Default window options. */
Options options_default();

/** A GLFW window together with the options it was created from. */
struct Window {
    GLFWwindow* handle;  ///< GLFW window handle.
    Options     options; ///< Creation options.
};

/**
 * @brief Create a window and OpenGL context with the given options.
 * @return True when the window was created successfully.
 */
bool create(Window* window, const Options* options);

/** @brief Destroy the window and its context. */
void destroy(Window* window);

/** @brief Return true when the window has been asked to close. */
bool should_close(const Window* window);

/** @brief Request the window to close (or cancel that request). */
void set_should_close(Window* window, bool value);

/** @brief Enable or disable vertical sync. */
void set_vsync(Window* window, bool enabled);

/** @brief Process pending window and input events. */
void poll_events(void);

/** @brief Swap the front and back buffers. */
void swap_buffers(Window* window);

/** @brief Query the framebuffer size in pixels. */
void framebuffer_size(const Window* window, i32* width, i32* height);

/**
 * @brief Initialize GLFW (with a default error callback).
 * @return True on success.
 */
bool init(void);

/** @brief Terminate GLFW. */
void terminate(void);

/**
 * @brief Load OpenGL function pointers via GLAD.
 * @return True on success.
 */
bool load_opengl(void);

/** @brief Create the ImGui context, enable docking and init the backends. */
void imgui_init(Window* window);

/** @brief Shut down the ImGui backends and destroy the context. */
void imgui_shutdown(void);

/** @brief Begin a frame: poll events, clear the backbuffer, start ImGui. */
void begin_frame(Window* window);

/** @brief End a frame: render ImGui and swap buffers. */
void end_frame(Window* window);

/** @brief True while `key` is held down. */
bool key_pressed(const Window* window, i32 key);

} // namespace Window
} // namespace Engine
