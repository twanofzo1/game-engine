#include "core/serialisation/serialiser.hpp"
#include "core/serialisation/deserialiser.hpp"
#include "frontend/core/window/window.hpp"



Window create_window(){
    if (!Window::init())
    {
        fprintf(stderr, "Failed to initialize window\n");
        exit(1);
    }

    std::optional<Window> optional_window = Window::make({
        .size = Vec2i::make({.x = 800, .y = 600}),
        .position = Vec2i::make({.x = -1, .y = -1}),
        .vsync = true,
        .resizable = true,
        .borderless = false,
        .screen_mode = ScreenMode::Windowed,
        .title = "My Window"
    });

    Window window = optional_window.value_or(Window{});
    if (!optional_window.has_value())
    {
        fprintf(stderr, "Failed to create window\n");
        exit(1);
    }

    if (!Window::load_opengl())
    {
        fprintf(stderr, "Failed to load OpenGL\n");
        Window::destroy(&window);
        Window::terminate();
        exit(1);
    }

    Window::imgui_init(&window);

    return window;
}



int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Window window = create_window();


    
    while (!Window::should_close(&window))
    {
        Window::begin_frame(&window);

        // Rendering code goes here...

        Window::end_frame(&window);
    }

    Window::imgui_shutdown();
    Window::destroy(&window);
    Window::terminate();
    return 0;
}