/**
 * @file window.cpp
 * @brief GLFW window wrapper implementation.
 */
#include "platform/window/window.hpp"

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <string.h>

namespace Engine {
namespace Window {

Options options_default()
{
    Options options;
    options.width       = 1280;
    options.height      = 720;
    options.position_x  = -1; /* -1: let the window manager choose. */
    options.position_y  = -1;
    options.vsync       = false;
    options.resizable   = true;
    options.borderless  = false;
    options.screen_mode = 0;
    strncpy(options.title, "Game", WINDOW_TITLE_MAX - 1);
    options.title[WINDOW_TITLE_MAX - 1] = '\0';
    return options;
}

bool create(Window* window, const Options* options)
{
    window->options = *options;
    window->handle  = NULL;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, options->resizable ? GLFW_TRUE : GLFW_FALSE);

    GLFWmonitor* monitor  = NULL;
    i32 width  = options->width;
    i32 height = options->height;
    bool undecorated = options->borderless;

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if ((options->screen_mode == 1 || options->screen_mode == 2) && primary)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(primary);
        width  = mode->width;
        height = mode->height;
        if (options->screen_mode == 1)
            monitor = primary;  /* exclusive fullscreen */
        else
            undecorated = true; /* borderless windowed fullscreen */
    }

    glfwWindowHint(GLFW_DECORATED, undecorated ? GLFW_FALSE : GLFW_TRUE);

    window->handle = glfwCreateWindow(width, height, options->title, monitor, NULL);
    if (!window->handle)
        return false;

    glfwMakeContextCurrent(window->handle);
    glfwSwapInterval(options->vsync ? 1 : 0);

    if (monitor == NULL)
    {
        if (options->screen_mode == 2)
            glfwSetWindowPos(window->handle, 0, 0);
        else if (options->position_x >= 0 && options->position_y >= 0)
            glfwSetWindowPos(window->handle, options->position_x, options->position_y);
    }

    return true;
}

void destroy(Window* window)
{
    if (window->handle)
    {
        glfwDestroyWindow(window->handle);
        window->handle = NULL;
    }
}

bool should_close(const Window* window)
{
    return glfwWindowShouldClose(window->handle) != 0;
}

void set_should_close(Window* window, bool value)
{
    glfwSetWindowShouldClose(window->handle, value ? GLFW_TRUE : GLFW_FALSE);
}

void set_vsync(Window* window, bool enabled)
{
    window->options.vsync = enabled;
    if (window->handle)
        glfwSwapInterval(enabled ? 1 : 0);
}

void poll_events(void)
{
    glfwPollEvents();
}

void swap_buffers(Window* window)
{
    glfwSwapBuffers(window->handle);
}

void framebuffer_size(const Window* window, i32* width, i32* height)
{
    glfwGetFramebufferSize(window->handle, width, height);
}

/** @brief GLFW error callback; prints the error to stderr. */
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

bool init(void)
{
    glfwSetErrorCallback(error_callback);
    return glfwInit() != 0;
}

void terminate(void)
{
    glfwTerminate();
}

bool load_opengl(void)
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0;
}

void imgui_init(Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->handle, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void imgui_shutdown(void)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void begin_frame(Window* window)
{
    poll_events();

    i32 fb_width = 0;
    i32 fb_height = 0;
    framebuffer_size(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
    glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void end_frame(Window* window)
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    swap_buffers(window);
}

bool key_pressed(const Window* window, i32 key)
{
    return glfwGetKey(window->handle, key) == GLFW_PRESS;
}

} // namespace Window
} // namespace Engine
