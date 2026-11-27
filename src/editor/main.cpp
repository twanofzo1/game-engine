#include "common/serialisation/serialiser.hpp"
#include "common/serialisation/deserialiser.hpp"
#include "common/platform/window/window.hpp"
#include "common/graphics/rendering/renderer.hpp"
#include "common/graphics/camera/camera.hpp"
#include "tools/shader_transpiler/shader_compiler.hpp"
#include "common/graphics/shader/shader.hpp"
#include "common/graphics/texture/texture.hpp"
#include "common/audio/audio.hpp"
#include "common/log/log.hpp"
#include "ui.hpp"
#include "imgui.h"
#include <glad/glad.h>

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

    std::optional<Window> optional_window = Window::create({
        .size = Vec2i::create({.x = 1280, .y = 720}),
        .position = Vec2i::create({.x = -1, .y = -1}),
        .vsync = true,
        .resizable = true,
        .borderless = false,
        .screen_mode = ScreenMode::Windowed,
        .title = "Editor"
    });

    Window window = optional_window.value_or(Window{});
    if (!optional_window.has_value())
    {
        log_error(LOG_SECTION_EDITOR, "failed to create window");
        exit(1);
    }

    return window;
}



int main(int argc, char** argv)
{
    (void)argc; (void)argv;

    Log::init();
    Log::set_global_level(LOG_LEVEL_INFO);

    Window window = create_window();

    bool audio_ok = Audio::init();
    if (!audio_ok) {
        log_warning(LOG_SECTION_AUDIO, "failed to initialise audio");
    }

    Renderer renderer = Renderer::create(0);


    std::optional<std::string> shader_src = read_file("resources/grass.shader");
    if (!shader_src.has_value())
    {
        log_error(LOG_SECTION_SHADER, "failed to read resources/grass.shader");
        window.destroy();
        exit(1);
    }

    std::expected<CompiledShader, CompileFailure> compiled =ShaderCompiler::compile(shader_src->c_str());
    if (!compiled.has_value())
    {
        log_error(LOG_SECTION_SHADER, "compilation failed (%d): %s at %u:%u",
                (int)compiled.error().error,
                compiled.error().message.c_str(),
                compiled.error().line,
                compiled.error().column);
        window.destroy();
        exit(1);
    }

    std::optional<Shader> shader_opt = Shader::create({
        .vs_source = compiled->vertex_source.c_str(),
        .fs_source = compiled->fragment_source.c_str()
    });
    if (!shader_opt.has_value())
    {
        log_error(LOG_SECTION_SHADER, "failed to create default shader");
        window.destroy();
        exit(1);
    }
    Shader shader = shader_opt.value();

   

    /* Load an image from disk and upload it as a GL texture. */
    std::optional<Texture> texture = Texture::load({
        .path = "resources/test.png",
    });
    if (!texture.has_value()) {
        log_error(LOG_SECTION_TEXTURE, "failed to load resources/test.png");
    } else {
        log_info(LOG_SECTION_TEXTURE, "loaded texture %u (%dx%d)", texture->id, texture->width, texture->height);
    }
    u32 texture_id = texture.has_value() ? texture->id : 0;

    /* Load a sound and set up a player instance for testing. */
    std::optional<SoundID> sound = Audio::load("resources/test.mp3");
    std::optional<PlayerID> player;
    if (sound.has_value()) {
        player = Audio::make_player({
            .sound = sound.value(),
            .volume = 1.0f,
            .pitch = 1.0f,
            .loop = true,
            .start = false,
        });
        if (!player.has_value()) {
            log_warning(LOG_SECTION_AUDIO, "failed to create audio player");
        }
    } else {
        log_warning(LOG_SECTION_AUDIO, "failed to load resources/test.mp3");
    }

    Camera camera = Camera::create({
        .position = Vec2::create({.x = 0.0f, .y = 0.0f}),
        .size = Vec2::create({.x = 1280.0f, .y = 720.0f})
    });

    log_debug(LOG_SECTION_EDITOR, "startup complete, entering main loop");

    while (!window.should_close())
    {
        window.begin_frame();

        shader.set_float("u_time", (f32)glfwGetTime());


        //camera.position.x = sin((f32)glfwGetTime()) * 400.0f;

        renderer.begin();
        //u32 xquads = 1000;
        //u32 yquads = 1000;
        //for (u32 i = 0; i < xquads; ++i){
        //    for (u32 j = 0; j < yquads; ++j){
        //        QuadConfig quad = {
        //            .position = Vec2::create({.x = (f32)(i * 2), .y = (f32)(j * 2)}),
        //            .size     = Vec2::create({.x = 2.0f, .y = 2.0f}),
        //            .color    = Color::create({
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
            .position = Vec2::create({.x = -200.0f, .y = -200.0f}),
            .size     = Vec2::create({.x = 100.0f, .y = 300.0f}),
            .color    = Color::create({.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f}),
            .texture  = texture_id,
        };

        QuadConfig quad2 = {
            .position = Vec2::create({.x = -200.0f, .y = -200.0f}),
            .size     = Vec2::create({.x = 100.0f, .y = 300.0f}),
            .color    = Color::create({.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f}),
            .shader  = shader.id,
        };
        renderer.push_quad(quad);
        renderer.push_quad(quad2);
        

        const ImGuiID dock_id = Ui::begin_dockspace(true);

        if (Ui::window({.title = "Stats", .size = ImVec2(280.0f, 120.0f)}, dock_id))
        {
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
        }
        Ui::end_window();

        if (Ui::window({.title = "Audio Player", .size = ImVec2(280.0f, 180.0f)}, dock_id))
        {
            if (player.has_value()) {
                static f32 volume = 1.0f;
                static f32 pitch = 1.0f;
                if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
                    Audio::set_volume(*player, volume);
                }
                if (ImGui::SliderFloat("Pitch", &pitch, 0.5f, 2.0f)) {
                    Audio::set_pitch(*player, pitch);
                }
                if (ImGui::Button("Play")) {
                    Audio::start(*player);
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop")) {
                    Audio::stop(*player);
                }
                ImGui::SameLine();
                if (ImGui::Button("Restart")) {
                    Audio::seek_to_start(*player);
                    Audio::start(*player);
                }
            } else {
                ImGui::TextDisabled("No audio player loaded");
            }
        }
        Ui::end_window();

        renderer.end();
        window.end_frame();
    }

    if (player.has_value()) {
        Audio::destroy_player(*player);
    }
    if (sound.has_value()) {
        Audio::unload(*sound);
    }
    Audio::shutdown();
    if (texture.has_value()) {
        texture.value().destroy();
    }
    window.destroy();
    
    return 0;
}