/**
 * @file window.cpp
 * @brief GLFW window wrapper implementation.
 */
#include "window.hpp"
#include "common/log/log.hpp"

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

u32 Window::count = 0;
bool Window::glfw_initialised = false;
bool Window::imgui_initialised = false;
bool Window::opengl_initialised = false;

/** @brief GLFW error callback; forwards the error to the logger. */
static void error_callback(int error, const char* description)
{
    log_error(LOG_SECTION_WINDOW, "GLFW error %d: %s", error, description);
}

bool Window::init_glfw()
{
    log_debug(LOG_SECTION_WINDOW, "init");
    glfwSetErrorCallback(error_callback);
    bool ok = glfwInit() != 0;
    if (!ok) {
        log_error(LOG_SECTION_WINDOW, "glfwInit failed");
    }
    Window::glfw_initialised = ok;
    return ok;
}

bool Window::init_opengl(void)
{
    log_debug(LOG_SECTION_WINDOW, "load_opengl");
    bool ok = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0;
    if (!ok) {
        log_error(LOG_SECTION_WINDOW, "gladLoadGLLoader failed");
    }
    Window::opengl_initialised = ok;
    return ok;
}

void Window::imgui_init(Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->handle, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Window::imgui_initialised = true;
}

void Window::imgui_destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Window::imgui_initialised = false;
}

void Window::cleanup()
{
    log_debug(LOG_SECTION_WINDOW, "terminate");
    imgui_destroy();
    glfwTerminate();
    Window::glfw_initialised = false;
    Window::imgui_initialised = false;
    Window::opengl_initialised = false;
}

/** @brief Create a window from the supplied configuration. */
std::optional<Window> Window::create(WindowConfig config)
{
    if (!Window::glfw_initialised)
    {
        if (!Window::init_glfw())
            return std::nullopt;
    }

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
    if (!window.handle) {
        log_error(LOG_SECTION_WINDOW, "glfwCreateWindow failed");
        return std::nullopt;
    }

    glfwMakeContextCurrent(window.handle);
    glfwSwapInterval(config.vsync ? 1 : 0);

    if (monitor == NULL)
    {
        if (config.screen_mode == ScreenMode::BorderlessFullscreen)
            glfwSetWindowPos(window.handle, 0, 0);
        else if (config.position.x >= 0 && config.position.y >= 0)
            glfwSetWindowPos(window.handle, config.position.x, config.position.y);
    }

    if (!Window::opengl_initialised)
    {
        if (!Window::init_opengl())
            return std::nullopt;
    }

    if (!Window::imgui_initialised)
    {
        Window::imgui_init(&window);
    }

    log_debug(LOG_SECTION_WINDOW, "created %dx%d", width, height);
    return window;
}

void Window::destroy()
{
    log_debug(LOG_SECTION_WINDOW, "destroy");
    if (this->handle)
    {
        glfwDestroyWindow(this->handle);
        Window::count -= 1;
        if (Window::count == 0)
        {
            Window::cleanup();
        }
    }
}

bool Window::should_close()
{
    return glfwWindowShouldClose(this->handle) != 0;
}

void Window::set_should_close(bool value)
{
    glfwSetWindowShouldClose(this->handle, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::set_vsync(bool enabled)
{
    this->vsync = enabled;
    if (this->handle)
        glfwSwapInterval(enabled ? 1 : 0);
}

void Window::poll_events(void)
{
    glfwPollEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers(this->handle);
}

Vec2i Window::get_framebuffer_size()
{
    Vec2i size = Vec2i::create({.x = 0, .y = 0});
    glfwGetFramebufferSize(this->handle, &size.x, &size.y);
    return size;
}

/** @brief Begin a frame: poll events, clear, and start ImGui. */
void Window::begin_frame()
{
    poll_events();

    Vec2i fb_size = get_framebuffer_size();
    glViewport(0, 0, fb_size.x, fb_size.y);
    glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/** @brief End a frame: render ImGui and swap buffers. */
void Window::end_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    swap_buffers();
}

bool Window::get_key_pressed(const i32 key)
{
    return glfwGetKey(this->handle, key) == GLFW_PRESS;
}