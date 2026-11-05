/**
 * @file errors.cpp
 * @brief Error popup handling for the editor.
 */
#include "services/errors.hpp"

#include "imgui.h"

#include <stdio.h>
#include <string.h>

namespace Editor {

void error_raise(Editor* editor, const char* message)
{
    snprintf(editor->errors.error_message, EDITOR_MAX_PATH, "%s", message);
    editor->errors.error_active = true;
}

void error_draw(Editor* editor)
{
    if (!editor->errors.error_active)
        return;

    ImGui::OpenPopup("Error");
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("%s", editor->errors.error_message);
        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            editor->errors.error_active = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool error_active(const Editor* editor)
{
    return editor->errors.error_active;
}

} // namespace Editor
