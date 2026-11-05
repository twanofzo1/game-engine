/**
 * @file notifications.cpp
 * @brief Non-blocking notification queue for the editor.
 */
#include "services/notifications.hpp"

#include "imgui.h"

#include <stdio.h>
#include <string.h>

namespace Editor {

void notify(Editor* editor, const char* message)
{
    i32 slot = (editor->notifications.notification_head + editor->notifications.notification_count) % EDITOR_MAX_NOTIFICATIONS;
    snprintf(editor->notifications.notifications[slot], EDITOR_MAX_PATH, "%s", message);

    if (editor->notifications.notification_count < EDITOR_MAX_NOTIFICATIONS)
        editor->notifications.notification_count++;
    else
        editor->notifications.notification_head = (editor->notifications.notification_head + 1) % EDITOR_MAX_NOTIFICATIONS;
}

void notifications_draw(Editor* editor)
{
    if (editor->notifications.notification_count == 0)
        return;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 pos(viewport->WorkPos.x + viewport->WorkSize.x - 320.0f,
               viewport->WorkPos.y + viewport->WorkSize.y - 40.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f), ImGuiCond_Always);
    ImGui::Begin("##notifications", NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing);

    for (i32 i = 0; i < editor->notifications.notification_count; ++i)
    {
        i32 idx = (editor->notifications.notification_head + i) % EDITOR_MAX_NOTIFICATIONS;
        ImGui::TextUnformatted(editor->notifications.notifications[idx]);
    }

    ImGui::End();
}

} // namespace Editor
