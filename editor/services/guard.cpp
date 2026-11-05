/**
 * @file guard.cpp
 * @brief Unsaved-changes guard implementation.
 */
#include "services/guard.hpp"
#include "editors/animations.hpp"
#include "editors/attacks.hpp"
#include "services/undo.hpp"

#include "imgui.h"

#include <stdio.h>
#include <string.h>

namespace Editor {

void mark_dirty(Editor* editor)
{
    /* Snapshot once at the start of a dirty cycle for undo. */
    if (!editor->guard.dirty)
        undo_push(editor);
    editor->guard.dirty = true;
}

/** @brief Perform the pending action. */
static void guard_perform(Editor* editor)
{
    PendingAction action = editor->guard.pending_action;
    editor->guard.pending_action = PENDING_NONE;

    switch (action)
    {
        case PENDING_OPEN_IMAGE:
            load_preview(editor, editor->guard.pending_path);
            break;
        case PENDING_OPEN_SPRITE:
            if (editor->guard.pending_index >= 0 && editor->guard.pending_index < editor->sprites.sprite_count)
            {
                editor->sprites.selected_sprite = editor->guard.pending_index;
                open_sprite(editor, &editor->sprites.sprites[editor->guard.pending_index]);
            }
            break;
        case PENDING_OPEN_ANIMATION:
            open_animation(editor, editor->guard.pending_index);
            break;
        case PENDING_NEW_ANIMATION:
            animation_clear(editor);
            break;
        case PENDING_OPEN_ATTACK:
            open_attack(editor, editor->guard.pending_index);
            break;
        case PENDING_NEW_ATTACK:
            attack_clear(editor);
            break;
        default:
            break;
    }
}

/** @brief Defer `action` when there are unsaved changes, else perform it. */
static void request(Editor* editor, PendingAction action)
{
    editor->guard.pending_action = action;
    if (!editor->guard.dirty)
        guard_perform(editor);
}

void request_open_image(Editor* editor, const char* path)
{
    snprintf(editor->guard.pending_path, EDITOR_MAX_PATH, "%s", path);
    request(editor, PENDING_OPEN_IMAGE);
}

void request_open_sprite(Editor* editor, i32 sprite_index)
{
    editor->guard.pending_index = sprite_index;
    request(editor, PENDING_OPEN_SPRITE);
}

void request_open_animation(Editor* editor, i32 anim_index)
{
    editor->guard.pending_index = anim_index;
    request(editor, PENDING_OPEN_ANIMATION);
}

void request_new_animation(Editor* editor)
{
    request(editor, PENDING_NEW_ANIMATION);
}

void request_open_attack(Editor* editor, i32 attack_index)
{
    editor->guard.pending_index = attack_index;
    request(editor, PENDING_OPEN_ATTACK);
}

void request_new_attack(Editor* editor)
{
    request(editor, PENDING_NEW_ATTACK);
}

void guard_draw(Editor* editor)
{
    if (editor->guard.pending_action == PENDING_NONE)
        return;

    ImGui::OpenPopup("Unsaved Changes");
    if (ImGui::BeginPopupModal("Unsaved Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("You have unsaved changes.");
        ImGui::Text("Save before switching?");

        if (ImGui::Button("Save", ImVec2(120.0f, 0.0f)))
        {
            if (editor->state == STATE_ANIMATION)
                save_animation(editor, editor->config_path);
            else if (editor->state == STATE_ATTACK)
                save_attack(editor, editor->config_path);
            else
                save_sprite(editor, editor->config_path);

            if (editor->guard.dirty)
            {
                /* Save failed (e.g. missing name); cancel so the error shows. */
                editor->guard.pending_action = PENDING_NONE;
            }
            else
            {
                guard_perform(editor);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard", ImVec2(120.0f, 0.0f)))
        {
            editor->guard.dirty = false;
            guard_perform(editor);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            editor->guard.pending_action = PENDING_NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

} // namespace Editor
