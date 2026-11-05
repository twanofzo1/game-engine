/**
 * @file main.cpp
 * @brief YAML editor entry point.
 *
 * Opens a GLFW window with an OpenGL 3.3 core context, initializes Dear ImGui
 * and runs the editor UI as a dockspace with three dockable windows: "Assets"
 * (file tree + saved sprites, left), "Viewport" (sprite preview, center) and
 * "Options" (sprite settings and save button, right).
 *
 * Usage:
 *   editor                 # interactive window
 *   editor --frames N      # render exactly N frames, then exit
 */

#include "imgui.h"
#include "imgui_internal.h"

#include "platform/window/window.hpp"
#include "core/editor.hpp"
#include "editors/animations.hpp"
#include "editors/attacks.hpp"
#include "editors/shapes.hpp"
#include "services/errors.hpp"
#include "services/notifications.hpp"
#include "services/guard.hpp"
#include "services/undo.hpp"
#include "ui/panels.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace Engine;

int main(int argc, char **argv)
{

    if (!Window::init())
    {
        fprintf(stderr, "error: window init failed\n");
        return 1;
    }

    Window::Options options = Window::options_default();
    options.width  = 960;
    options.height = 600;
    strncpy(options.title, "YAML Editor", WINDOW_TITLE_MAX - 1);
    options.title[WINDOW_TITLE_MAX - 1] = '\0';

    Window::Window window;
    if (!Window::create(&window, &options))
    {
        fprintf(stderr, "error: window create failed\n");
        Window::terminate();
        return 1;
    }

    if (!Window::load_opengl())
    {
        fprintf(stderr, "error: opengl load failed\n");
        Window::destroy(&window);
        Window::terminate();
        return 1;
    }

    Window::imgui_init(&window);

    Editor::Editor editor;
    Editor::init(&editor);

    Editor::UndoState undo_stack;
    Editor::undo_init(&undo_stack);
    editor.undo = &undo_stack;

    snprintf(editor.config_path, EDITOR_MAX_PATH, "resources/config.yaml");
    Editor::scan_directory(&editor, "resources");
    Editor::load_sprites(&editor, editor.config_path);
    Editor::load_animations(&editor, editor.config_path);
    Editor::load_attacks(&editor, editor.config_path);
    Editor::load_shapes(&editor, editor.config_path);

    int frame = 0;

    while (!Window::should_close(&window))
    {
        Window::begin_frame(&window);

        if (Window::key_pressed(&window, GLFW_KEY_ESCAPE))
            Window::set_should_close(&window, true);

        /* Undo / redo shortcuts (Ctrl+Z, Ctrl+Shift+Z, Ctrl+Y). */
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.KeyCtrl && !ImGui::IsAnyItemActive())
            {
                if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
                {
                    if (io.KeyShift)
                        Editor::redo_do(&editor);
                    else
                        Editor::undo_do(&editor);
                }
                else if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
                {
                    Editor::redo_do(&editor);
                }
            }
        }

        /* Menu bar. */
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    Window::set_should_close(&window, true);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        /* Fullscreen dockspace host window. */
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::Begin("##dockhost", NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");

        /* Build the default layout only if no saved layout exists yet. */
        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            ImGuiID left;
            ImGuiID center;
            ImGuiID right;
            left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, NULL, &center);
            right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.28f, NULL, &center);

            ImGuiID assets_node;
            ImGuiID sprites_node;
            assets_node = ImGui::DockBuilderSplitNode(left, ImGuiDir_Up, 0.45f, NULL, &sprites_node);

            ImGui::DockBuilderDockWindow("Assets", assets_node);
            ImGui::DockBuilderDockWindow("Sprites", sprites_node);
            ImGui::DockBuilderDockWindow("Viewport", center);
            ImGui::DockBuilderDockWindow("Animations", center);
            ImGui::DockBuilderDockWindow("Attacks", center);
            ImGui::DockBuilderDockWindow("Options", right);
            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        ImGui::End();

        /* Dockable panels. */
        Editor::draw_assets(&editor);
        Editor::draw_sprites(&editor);
        Editor::draw_viewport(&editor);
        Editor::draw_options(&editor);
        Editor::draw_animations(&editor);
        Editor::draw_attacks(&editor);

        /* Bring the matching editor tab to front when the state changes. */
        static i32 last_state = -1;
        if ((i32)editor.state != last_state)
        {
            last_state = (i32)editor.state;
            if (editor.state == Editor::STATE_ANIMATION)
                ImGui::SetWindowFocus("Animations");
            else if (editor.state == Editor::STATE_ATTACK)
                ImGui::SetWindowFocus("Attacks");
            else
                ImGui::SetWindowFocus("Viewport");
        }

        /* Overlays: unsaved-changes guard, error popup and notifications. */
        Editor::guard_draw(&editor);
        Editor::error_draw(&editor);
        Editor::notifications_draw(&editor);

        Window::end_frame(&window);

        if (max_frames > 0 && ++frame >= max_frames)
            break;
    }

    Editor::undo_free(&undo_stack);

    Editor::destroy(&editor);

    Window::imgui_shutdown();

    Window::destroy(&window);
    Window::terminate();
    return 0;
}
