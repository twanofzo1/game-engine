/**
 * @file ui.hpp
 * @brief Small helpers built on Dear ImGui for laying out dockable windows.
 *
 * Wraps the boilerplate around ImGui dockspaces, menu bars and dockable
 * windows so editor panels can be created with a single call. The docking
 * flag is already enabled in Window::imgui_init(), these helpers just make
 * the layout code tidy and consistent.
 */
#pragma once

#include "imgui.h"

#include <stdint.h>

/** Namespace for editor ImGui layout helpers. */
namespace Ui
{
    /** Options passed inline when opening a dockable window. */
    struct WindowConfig
    {
        const char* title = "Window";      ///< Window title shown in the tab bar; also keys its open state.
        ImGuiWindowFlags flags = 0;        ///< Extra ImGui window flags.
        ImVec2 size = ImVec2(0.0f, 0.0f);  ///< Initial window size (0 = auto).
        ImVec2 position = ImVec2(0.0f, 0.0f); ///< Initial window position (0 = auto).
    };

    /**
     * @brief Begin a dockable window, persisting its open state internally.
     *
     * The open/close state is stored inside the module keyed by `config.title`,
     * so the caller never holds a pointer or static object. Pass the config
     * inline each frame: `if (Ui::window({.title = "Stats"}, dock_id)) { ... }`.
     *
     * @param config  Window options passed inline.
     * @param dock_id Id of the docking node returned by begin_dockspace().
     * @return True while the window is visible and its contents should be emitted.
     */
    bool window(WindowConfig config, ImGuiID dock_id);

    /** @brief End a window opened with window(). */
    void end_window(void);

    /**
     * @brief Start a full-window dockspace (hosts every dockable window).
     * @param menu_bar True to also draw a top menu bar as part of the dockspace.
     * @return The dockspace id that windows should be docked into.
     */
    ImGuiID begin_dockspace(bool menu_bar);

    void close_window(const char* title);
};
