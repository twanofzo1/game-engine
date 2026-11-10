/**
 * @file window.hpp
 * @brief Declares the GLFW window wrapper and its creation options.
 */
#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "core/types.hpp"
#include <optional>
#include <string>


enum class ScreenMode {
    Windowed = 0,
    Fullscreen = 1,
    BorderlessFullscreen = 2
};

/** Options used when creating a window. */
struct WindowConfig {
    Vec2i size = Vec2i::make({.x = 800, .y = 600}); ///< Initial window size in pixels.
    Vec2i position = Vec2i::make({.x = -1, .y = -1}); ///< Initial window position in pixels (-1 = let the OS decide).
    bool vsync = true;                             ///< Enable vertical sync.
    bool resizable = true;                         ///< Allow the window to be resized.
    bool borderless = false;                       ///< Hide the title bar and borders.
    ScreenMode screen_mode = ScreenMode::Windowed; ///< Window mode: windowed, fullscreen, or borderless fullscreen.
    std::string title = "My Window";               ///< Window title.
};



/** A GLFW window together with the options it was created from. */
struct Window {
    GLFWwindow* handle;     ///< GLFW window handle.
    Vec2i size;             ///< Current window size in pixels.
    Vec2i position;         ///< Current window position in pixels.
    bool vsync;             ///< Whether vertical sync is enabled.
    bool resizable;         ///< Whether the window is resizable.
    bool borderless;        ///< Whether the window is borderless.
    ScreenMode screen_mode; ///< Current window mode.
    std::string title;      ///< Current window title.


    static std::optional<Window> make(WindowConfig config);
    static void destroy(Window* window);
    static bool should_close(const Window* window);
    static void set_should_close(Window* window, bool value);
    static void set_vsync(Window* window, bool enabled);
    static void poll_events(void);
    static void swap_buffers(Window* window);
    static Vec2i framebuffer_size(const Window* window);
    static bool init(void);
    static void terminate(void);
    static bool load_opengl(void);
    static void imgui_init(Window* window);
    static void imgui_shutdown(void);
    static void begin_frame(Window* window);
    static void end_frame(Window* window);
    static bool key_pressed(const Window* window, i32 key);
};


 