#include "core/serialisation/serialiser.hpp"
#include "core/serialisation/deserialiser.hpp"
#include "core/frontend/window/window.hpp"
#include "core/frontend/rendering/renderer.hpp"
#include "core/frontend/camera/camera.hpp"
#include "core/frontend/shader_parsing/shader_compiler.hpp"
#include "core/frontend/rendering/shader.hpp"
#include "imgui.h"
#include <glad/glad.h>

#include <stb_image.h>

#include <expected>
#include <stdio.h>


/**
 * @brief Read an entire file into a string.
 * @return The file contents, or nullopt on failure.
 */
static std::optional<std::string> read_file(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == nullptr) {
        return std::nullopt;
    }

    std::string contents;
    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        contents.append(buffer, n);
    }
    fclose(f);
    return contents;
}


Window create_window(){
    if (!Window::init())
    {
        fprintf(stderr, "Failed to initialize window\n");
        exit(1);
    }

    std::optional<Window> optional_window = Window::make({
        .size = Vec2i::make({.x = 800, .y = 600}),
        .position = Vec2i::make({.x = -1, .y = -1}),
        .vsync = false,
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

    std::expected<Renderer, RendererError> renderer_result = Renderer::make();
    if (!renderer_result)
    {
        fprintf(stderr, "Failed to create renderer: %d\n", (int)renderer_result.error());
        Window::terminate();
        exit(1);
    }
    Renderer renderer = *renderer_result;



    std::optional<std::string> shader_src = read_file("resources/grass.shader");
    if (!shader_src.has_value())
    {
        fprintf(stderr, "Failed to read resources/test.shader\n");
        Window::terminate();
        exit(1);
    }

    std::expected<CompiledShader, CompileFailure> compiled =ShaderCompiler::compile(shader_src->c_str());
    if (!compiled.has_value())
    {
        fprintf(stderr, "Shader compilation failed (stage %d): %s at %u:%u\n",
                (int)compiled.error().error,
                compiled.error().message.c_str(),
                compiled.error().line,
                compiled.error().column);
        Window::terminate();
        exit(1);
    }

    std::optional<ShaderID> shader = Shader::make({
        .vs_source = compiled->vertex_source.c_str(),
        .fs_source = compiled->fragment_source.c_str()
    });
    if (!shader.has_value())
    {
        fprintf(stderr, "Failed to create default shader\n");
        Renderer::shutdown(&renderer);
        Window::terminate();
        exit(1);
    }

   

    /* Load an image with stb_image and upload it as a GL texture. */
    u32 texture_id = 0;
    {
        int width = 0, height = 0, channels = 0;
        u8* pixels = stbi_load("resources/test.png", &width, &height, &channels, 4);
        if (pixels) {
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(pixels);
            printf("Loaded texture %u (%dx%d)\n", texture_id, width, height);
        } else {
            fprintf(stderr, "Failed to load resources/test.png\n");
        }
    }

    Camera camera = Camera::make({
        .position = Vec2::make({.x = 0.0f, .y = 0.0f}),
        .size = Vec2::make({.x = 1280.0f, .y = 720.0f})
    });


    while (!Window::should_close(&window))
    {
        Window::begin_frame(&window);

        Shader::set_float(shader.value_or(0), "u_time", (f32)glfwGetTime());


        //camera.position.x = sin((f32)glfwGetTime()) * 400.0f;

        Renderer::begin(&renderer, &camera);
        //u32 xquads = 1000;
        //u32 yquads = 1000;
        //for (u32 i = 0; i < xquads; ++i){
        //    for (u32 j = 0; j < yquads; ++j){
        //        QuadConfig quad = {
        //            .position = Vec2::make({.x = (f32)(i * 2), .y = (f32)(j * 2)}),
        //            .size     = Vec2::make({.x = 2.0f, .y = 2.0f}),
        //            .color    = Color::make({
        //                .r = 0.0f + 0.005f * (f32)i,
        //                .g = 0.0f + 0.002f * (f32)j,
        //                .b = 0.0f + 0.001f * (f32)j,
        //                .a = 1.0f
        //            }),
        //        };
        //        Renderer::push_quad(&renderer, &quad);
        //    }
        //}

        QuadConfig quad = {
            .position = Vec2::make({.x = -200.0f, .y = -200.0f}),
            .size     = Vec2::make({.x = 100.0f, .y = 300.0f}),
            .color    = Color::make({.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f}),
            .texture  = texture_id,
            .shader_id = shader.value_or(0),
            .u0 = 0.0f, .v0 = 0.0f,
            .u1 = 1.0f, .v1 = 1.0f,
        };
        Renderer::push_quad(&renderer, &quad);
        

        ImGui::SetNextWindowPos({0.0,0.0});
        ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        static double accumulated_time = 0.0;
        static int frame_count = 0;
        static double fps = 0.0;
        frame_count++;
        accumulated_time += ImGui::GetIO().DeltaTime;
        if (accumulated_time >= 0.2) {
            fps = (double)frame_count / accumulated_time;
            frame_count = 0;
            accumulated_time = 0.0;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
        ImGui::End();

        Renderer::end(&renderer);

        Window::end_frame(&window);
    }

    Renderer::shutdown(&renderer);
    Window::imgui_shutdown();
    Window::destroy(&window);
    Window::terminate();
    return 0;
}