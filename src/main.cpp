#include "core/serialisation/serialiser.hpp"
#include "core/serialisation/deserialiser.hpp"
#include "core/frontend/window/window.hpp"
#include "core/frontend/rendering/renderer.hpp"
#include "core/frontend/camera/camera.hpp"



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
    (void)argc; (void)argv;

    Window window = create_window();

    Renderer renderer;
    Renderer::init(&renderer);

    Camera camera = Camera::make(
        Vec2::make({.x = 0.0f, .y = 0.0f}),
        Vec2::make({.x = 1280.0f, .y = 720.0f}));

    while (!Window::should_close(&window))
    {
        Window::begin_frame(&window);

        Renderer::begin(&renderer, &camera);

        QuadConfig quad = {
            .color    = Color::make({
                .r = 0.9f, 
                .g = 0.2f, 
                .b = 0.3f, 
                .a = 1.0f
            }),
        };
        Renderer::push_quad(&renderer, &quad);

        Renderer::end(&renderer);

        Window::end_frame(&window);
    }

    Renderer::shutdown(&renderer);
    Window::imgui_shutdown();
    Window::destroy(&window);
    Window::terminate();
    return 0;
}