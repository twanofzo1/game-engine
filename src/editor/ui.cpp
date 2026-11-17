/**
 * @file ui.cpp
 * @brief ImGui dockspace / dockable window helpers.
 */
#include "ui.hpp"

#include <string>
#include <unordered_map>

namespace
{
    /** Open state per window, keyed by title. Persists across frames. */
    std::unordered_map<std::string, bool> window_open_state;
}

/**
 * @brief Begin a full-window dockspace, optionally with a menu bar.
 */
ImGuiID Ui::begin_dockspace(bool menu_bar)
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dock_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dock_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    if (menu_bar)
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::EndMenuBar();
        }
    }

    ImGui::End();
    return dock_id;
}

/**
 * @brief Begin a dockable window, persisting its open state internally.
 */
bool Ui::window(WindowConfig config, ImGuiID dock_id)
{
    std::string key = config.title;
    bool open = true;
    auto it = window_open_state.find(key);
    if (it == window_open_state.end())
    {
        window_open_state.emplace(key, true);
    }
    else
    {
        open = it->second;
    }

    if (!open)
    {
        return false;
    }

    ImGui::SetNextWindowSize(config.size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(config.position, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowDockID(dock_id, ImGuiCond_FirstUseEver);

    ImGui::Begin(config.title, &open, config.flags);
    window_open_state[key] = open;
    return open;
}

/** @brief End a window opened with window(). */
void Ui::end_window(void)
{
    ImGui::End();
}


void Ui::close_window(const char* title)
{
    window_open_state[title] = false;
}