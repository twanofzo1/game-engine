/**
 * @file window.cpp
 * @brief GLFW window wrapper implementation.
 */
#include "window.hpp"

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>


/** @brief Create a window from the supplied configuration. */
std::optional<Window> Window::make(WindowConfig config)
{
    Window window = {};
    window.size        = config.size;
    window.position    = config.position;
    window.vsync       = config.vsync;
    window.resizable   = config.resizable;
    window.borderless  = config.borderless;
    window.screen_mode = config.screen_mode;
    window.title       = config.title;
    window.handle      = NULL;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);

    GLFWmonitor* monitor  = NULL;
    i32 width  = config.size.x;
    i32 height = config.size.y;
    bool undecorated = config.borderless;

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if ((config.screen_mode == ScreenMode::Fullscreen ||
         config.screen_mode == ScreenMode::BorderlessFullscreen) && primary)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(primary);
        width  = mode->width;
        height = mode->height;
        if (config.screen_mode == ScreenMode::Fullscreen)
            monitor = primary;  /* exclusive fullscreen */
        else
            undecorated = true; /* borderless windowed fullscreen */
    }

    glfwWindowHint(GLFW_DECORATED, undecorated ? GLFW_FALSE : GLFW_TRUE);

    window.handle = glfwCreateWindow(width, height, config.title.c_str(), monitor, NULL);
    if (!window.handle)
        return std::nullopt;

    glfwMakeContextCurrent(window.handle);
    glfwSwapInterval(config.vsync ? 1 : 0);

    if (monitor == NULL)
    {
        if (config.screen_mode == ScreenMode::BorderlessFullscreen)
            glfwSetWindowPos(window.handle, 0, 0);
        else if (config.position.x >= 0 && config.position.y >= 0)
            glfwSetWindowPos(window.handle, config.position.x, config.position.y);
    }

    return window;
}

void Window::destroy(Window* window)
{
    if (window->handle)
    {
        glfwDestroyWindow(window->handle);
        window->handle = NULL;
    }
    
}

bool Window::should_close(const Window* window)
{
    return glfwWindowShouldClose(window->handle) != 0;
}

void Window::set_should_close(Window* window, bool value)
{
    glfwSetWindowShouldClose(window->handle, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_vsync(Window* window, bool enabled)
{
    window->vsync = enabled;
    if (window->handle)
        glfwSwapInterval(enabled ? 1 : 0);
}

void Window::poll_events(void)
{
    glfwPollEvents();
}

void Window::swap_buffers(Window* window)
{
    glfwSwapBuffers(window->handle);
}

Vec2i Window::framebuffer_size(const Window* window)
{
    Vec2i size = Vec2i::make({.x = 0, .y = 0});
    glfwGetFramebufferSize(window->handle, &size.x, &size.y);
    return size;
}

/** @brief GLFW error callback; prints the error to stderr. */
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

bool Window::init(void)
{
    glfwSetErrorCallback(error_callback);
    return glfwInit() != 0;
}

void Window::terminate(void)
{
    glfwTerminate();
}

bool Window::load_opengl(void)
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0;
}

void Window::imgui_init(Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->handle, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Window::imgui_shutdown(void)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/** @brief Begin a frame: poll events, clear, and start ImGui. */
void Window::begin_frame(Window* window)
{
    poll_events();

    Vec2i fb_size = framebuffer_size(window);
    glViewport(0, 0, fb_size.x, fb_size.y);
    glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/** @brief End a frame: render ImGui and swap buffers. */
void Window::end_frame(Window* window)
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    swap_buffers(window);
}

bool Window::key_pressed(const Window* window, i32 key)
{
    return glfwGetKey(window->handle, key) == GLFW_PRESS;
}


