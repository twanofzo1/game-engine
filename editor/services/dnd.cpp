/**
 * @file dnd.cpp
 * @brief Dear ImGui drag-and-drop helpers for the editor's reference slots.
 */
#include "services/dnd.hpp"

#include "imgui.h"

#include <stdio.h>
#include <string.h>

namespace Editor {

bool dnd_slot_i32(const char* label, const char* payload, const char* value_text,
                  i32* value)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    char id[160];
    snprintf(id, sizeof(id), "%s###slot_%s", value_text, label);
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::Button(id, ImVec2(avail.x, 0.0f));

    bool dropped = false;
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(payload))
        {
            if (p->DataSize >= (int)sizeof(i32))
            {
                *value = *(const i32*)p->Data;
                dropped = true;
            }
        }
        ImGui::EndDragDropTarget();
    }
    return dropped;
}

bool dnd_slot_string(const char* label, const char* payload, const char* value_text,
                     char* out, i32 size)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    char id[288];
    snprintf(id, sizeof(id), "%s###slot_%s", value_text, label);
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::Button(id, ImVec2(avail.x, 0.0f));

    bool dropped = false;
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(payload))
        {
            if (p->DataSize > 0 && out && size > 0)
            {
                snprintf(out, size, "%s", (const char*)p->Data);
                dropped = true;
            }
        }
        ImGui::EndDragDropTarget();
    }
    return dropped;
}

void dnd_source_i32(const char* payload, i32 value, const char* preview)
{
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload(payload, &value, sizeof(value));
        ImGui::TextUnformatted(preview);
        ImGui::EndDragDropSource();
    }
}

void dnd_source_string(const char* payload, const char* value)
{
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload(payload, value, strlen(value) + 1);
        ImGui::TextUnformatted(value);
        ImGui::EndDragDropSource();
    }
}

} // namespace Editor
