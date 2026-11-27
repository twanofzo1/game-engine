/**
 * @file window.hpp
 * @brief Declares the GLFW window wrapper and its creation options.
 */
#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "common/core/types.hpp"
#include <optional>
#include <string>


enum class ScreenMode {
    Windowed = 0,
    Fullscreen = 1,
    BorderlessFullscreen = 2
};

/** Options used when creating a window. */
struct WindowConfig {
    Vec2i size = Vec2i::create({.x = 800, .y = 600}); ///< Initial window size in pixels.
    Vec2i position = Vec2i::create({.x = -1, .y = -1}); ///< Initial window position in pixels (-1 = let the OS decide).
    bool vsync = true;                             ///< Enable vertical sync.
    bool resizable = true;                         ///< Allow the window to be resized.
    bool borderless = false;                       ///< Hide the title bar and borders.
    ScreenMode screen_mode = ScreenMode::Windowed; ///< Window mode: windowed, fullscreen, or borderless fullscreen.
    std::string title = "My Window";               ///< Window title.
};



/** A GLFW window together with the options it was created from. */
class Window {
private:
    static bool glfw_initialised;
    static bool imgui_initialised;
    static bool opengl_initialised;
    static u32 count;

    GLFWwindow* handle;     ///< GLFW window handle.
    Vec2i size;             ///< Current window size in pixels.
    Vec2i position;         ///< Current window position in pixels.
    bool vsync;             ///< Whether vertical sync is enabled.
    bool resizable;         ///< Whether the window is resizable.
    bool borderless;        ///< Whether the window is borderless.
    ScreenMode screen_mode; ///< Current window mode.
    std::string title;      ///< Current window title.
private:
    static bool init_opengl();
    static bool init_glfw();
    static void imgui_init(Window* window);
    static void imgui_destroy();
    static void cleanup();

public:
    static std::optional<Window> create(WindowConfig config);
    void destroy();
    bool should_close();
    void set_should_close(bool value);
    void set_vsync(bool enabled);
    void poll_events();
    void swap_buffers();
    Vec2i get_framebuffer_size();
    void begin_frame();
    void end_frame();
    bool get_key_pressed(i32 key);
};


 