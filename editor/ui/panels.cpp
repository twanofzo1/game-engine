/**
 * @file panels.cpp
 * @brief Dear ImGui panels for the editor: assets, viewport and options.
 */
#include "ui/panels.hpp"
#include "editors/animations.hpp"
#include "editors/attacks.hpp"
#include "services/dnd.hpp"
#include "services/guard.hpp"
#include "viewport/viewport_edit.hpp"

#include "imgui.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

using namespace Engine; /* usage code, like src/main.cpp */

namespace Editor {

/** @brief Clamp `v` to the inclusive range [lo, hi]. */
static i32 clamp_i(i32 v, i32 lo, i32 hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void draw_assets(Editor* editor)
{
    ImGui::Begin("Assets");
    ImGui::Text("assets: %s", editor->assets.directory);
    ImGui::Separator();

    for (i32 i = 0; i < editor->assets.file_count; ++i)
    {
        char full[EDITOR_MAX_PATH * 2];
        snprintf(full, sizeof(full), "%s/%s",
                 editor->assets.directory, editor->assets.files[i].name);

        if (ImGui::Selectable(editor->assets.files[i].name, editor->assets.selected_file == i))
        {
            editor->assets.selected_file = i;
            request_open_image(editor, full);
        }
        dnd_source_string(DND_IMAGE_PATH, full);
    }

    ImGui::End();
}

/** @brief Open the rename popup for a sprite. */
static void begin_rename_sprite(Editor* editor, const SpriteEntry* sprite)
{
    editor->rename.rename_is_animation = false;
    editor->rename.rename_is_attack = false;
    editor->rename.rename_index = sprite->yaml_index;
    snprintf(editor->rename.rename_name, EDITOR_SPRITE_NAME_MAX, "%s", sprite->name);
    editor->rename.rename_open = true;
}

/** @brief Open the rename popup for an animation. */
static void begin_rename_animation(Editor* editor, const AnimationEntry* anim)
{
    editor->rename.rename_is_animation = true;
    editor->rename.rename_is_attack = false;
    editor->rename.rename_index = anim->yaml_index;
    snprintf(editor->rename.rename_name, EDITOR_SPRITE_NAME_MAX, "%s", anim->name);
    editor->rename.rename_open = true;
}

/** @brief Open the rename popup for an attack. */
static void begin_rename_attack(Editor* editor, const AttackEntry* atk)
{
    editor->rename.rename_is_animation = false;
    editor->rename.rename_is_attack = true;
    editor->rename.rename_index = atk->yaml_index;
    snprintf(editor->rename.rename_name, EDITOR_SPRITE_NAME_MAX, "%s", atk->name);
    editor->rename.rename_open = true;
}

/** @brief Draw the rename popup (modal). */
static void draw_rename_modal(Editor* editor)
{
    if (editor->rename.rename_open)
    {
        editor->rename.rename_open = false;
        ImGui::OpenPopup("Rename");
    }

    if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const char* kind = editor->rename.rename_is_attack ? "attack"
                         : (editor->rename.rename_is_animation ? "animation" : "sprite");
        ImGui::Text("Rename %s", kind);
        ImGui::InputText("##rename_name", editor->rename.rename_name, EDITOR_SPRITE_NAME_MAX);
        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            if (editor->rename.rename_is_attack)
                rename_attack(editor, editor->config_path, editor->rename.rename_index,
                              editor->rename.rename_name);
            else if (editor->rename.rename_is_animation)
                rename_animation(editor, editor->config_path, editor->rename.rename_index,
                                 editor->rename.rename_name);
            else
                rename_sprite(editor, editor->config_path, editor->rename.rename_index,
                              editor->rename.rename_name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

/** @brief Draw one animation row inside a sprite folder. */
static void draw_animation_row(Editor* editor, i32 a)
{
    AnimationEntry* anim = &editor->animation.animations[a];
    char label[EDITOR_ANIMATION_NAME_MAX + 16];
    snprintf(label, sizeof(label), "%s [animation]###anim%d", anim->name, a);

    if (ImGui::Selectable(label, editor->animation.selected_animation == a))
        request_open_animation(editor, a);
    dnd_source_i32(DND_ANIMATION, anim->yaml_index, anim->name);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Rename"))
            begin_rename_animation(editor, anim);
        if (ImGui::MenuItem("Delete"))
            delete_animation_at(editor, editor->config_path, anim->yaml_index);
        ImGui::EndPopup();
    }
}

/** @brief Draw one sprite folder, with its animations nested inside. */
static void draw_sprite_folder(Editor* editor, i32 s)
{
    const SpriteEntry* sprite = &editor->sprites.sprites[s];
    const char* sname = sprite->name[0] ? sprite->name : sprite->path;
    char folder[EDITOR_SPRITE_NAME_MAX + EDITOR_MAX_PATH + 16];
    snprintf(folder, sizeof(folder), "%s [sprite]###spritefolder%d", sname, s);

    bool expanded = ImGui::TreeNode(folder);
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open"))
            request_open_sprite(editor, s);
        if (ImGui::MenuItem("Rename"))
            begin_rename_sprite(editor, sprite);
        if (ImGui::MenuItem("Delete"))
            delete_sprite_at(editor, editor->config_path, sprite->yaml_index);
        ImGui::EndPopup();
    }
    dnd_source_i32(DND_SPRITE, sprite->yaml_index, sname);

    if (expanded)
    {
        for (i32 a = 0; a < editor->animation.animation_count; ++a)
        {
            if (editor->animation.animations[a].sprite_id != sprite->yaml_index)
                continue;
            draw_animation_row(editor, a);
        }
        ImGui::TreePop();
    }
}

/** @brief Draw one attack row. */
static void draw_attack_row(Editor* editor, i32 a)
{
    AttackEntry* atk = &editor->attack.attacks[a];
    char label[EDITOR_SPRITE_NAME_MAX + 16];
    snprintf(label, sizeof(label), "%s [attack]###attack%d", atk->name, a);

    if (ImGui::Selectable(label, editor->attack.selected_attack == a))
        request_open_attack(editor, a);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Rename"))
            begin_rename_attack(editor, atk);
        if (ImGui::MenuItem("Delete"))
            delete_attack_at(editor, editor->config_path, atk->yaml_index);
        ImGui::EndPopup();
    }
}

void draw_sprites(Editor* editor)
{
    ImGui::Begin("Sprites");
    ImGui::Text("sprites (%d)", editor->sprites.sprite_count);
    ImGui::Separator();

    for (i32 s = 0; s < editor->sprites.sprite_count; ++s)
        draw_sprite_folder(editor, s);

    ImGui::Separator();
    ImGui::Text("attacks (%d)", editor->attack.attack_count);
    for (i32 a = 0; a < editor->attack.attack_count; ++a)
        draw_attack_row(editor, a);

    draw_rename_modal(editor);

    ImGui::End();
}

void draw_viewport(Editor* editor)
{
    /* Give the viewport a darker background so it stands out from the panels. */
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.02f, 0.03f, 1.0f));
    ImGui::Begin("Viewport");

    if (editor->preview.preview_loaded)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x >= 32.0f && avail.y >= 32.0f)
        {
            i32 fx = clamp_i(editor->preview.frames_x, 1, 256);
            i32 fy = clamp_i(editor->preview.frames_y, 1, 256);
            i32 tex_w = editor->preview.gpu.width;
            i32 tex_h = editor->preview.gpu.height;
            i32 total = fx * fy;
            i32 f = editor->preview.frame % total;
            i32 col = f % fx;
            i32 row = f / fx;

            /* Source rectangle: the full sheet, or only the selected frame. */
            f32 src_w = (f32)tex_w;
            f32 src_h = (f32)tex_h;
            ImVec2 uv0(0.0f, 0.0f);
            ImVec2 uv1(1.0f, 1.0f);
            if (editor->preview.single_frame)
            {
                uv0 = ImVec2((f32)col / (f32)fx, (f32)row / (f32)fy);
                uv1 = ImVec2((f32)(col + 1) / (f32)fx, (f32)(row + 1) / (f32)fy);
                src_w = (f32)tex_w / (f32)fx;
                src_h = (f32)tex_h / (f32)fy;
            }

            /* Fit the source to the viewport, preserving aspect ratio. */
            f32 scale = avail.x / src_w;
            if (src_h * scale > avail.y)
                scale = avail.y / src_h;
            scale *= editor->preview.zoom;

            ImVec2 sheet(src_w * scale, src_h * scale);

            /* Center the sheet when it is smaller than the viewport. */
            f32 ox = (avail.x - sheet.x) * 0.5f;
            f32 oy = (avail.y - sheet.y) * 0.5f;
            if (ox < 0.0f) ox = 0.0f;
            if (oy < 0.0f) oy = 0.0f;
            ImVec2 cursor = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(cursor.x + ox, cursor.y + oy));

            ImGui::InvisibleButton("##sheet", sheet);
            ImVec2 pmin = ImGui::GetItemRectMin();
            ImVec2 pmax = ImGui::GetItemRectMax();
            ImDrawList* dl = ImGui::GetWindowDrawList();

            ImTextureID tid = (ImTextureID)(intptr_t)editor->preview.gpu.id;
            dl->AddImage(tid, pmin, pmax, uv0, uv1);

            /* Attack editor: overlay the active hitbox + movement vector. */
            if (editor->state == STATE_ATTACK)
            {
                ImVec2 center((pmin.x + pmax.x) * 0.5f,
                              (pmin.y + pmax.y) * 0.5f);
                viewport_draw_attack_overlay(editor, dl, center, scale);
            }

            /* Grid, highlight and click-to-select only in sheet mode. */
            if (!editor->preview.single_frame)
            {
                for (i32 c = 1; c < fx; ++c)
                {
                    f32 x = pmin.x + sheet.x * (f32)c / (f32)fx;
                    dl->AddLine(ImVec2(x, pmin.y), ImVec2(x, pmax.y),
                                IM_COL32(255, 255, 255, 70), 1.0f);
                }
                for (i32 r = 1; r < fy; ++r)
                {
                    f32 y = pmin.y + sheet.y * (f32)r / (f32)fy;
                    dl->AddLine(ImVec2(pmin.x, y), ImVec2(pmax.x, y),
                                IM_COL32(255, 255, 255, 70), 1.0f);
                }

                ImVec2 cmin(pmin.x + sheet.x * (f32)col / (f32)fx,
                            pmin.y + sheet.y * (f32)row / (f32)fy);
                ImVec2 cmax(cmin.x + sheet.x / (f32)fx,
                            cmin.y + sheet.y / (f32)fy);
                dl->AddRect(cmin, cmax, IM_COL32(255, 210, 0, 230), 0.0f, 0, 2.0f);

                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                {
                    ImVec2 m = ImGui::GetMousePos();
                    i32 cc = clamp_i((i32)((m.x - pmin.x) / (sheet.x / (f32)fx)), 0, fx - 1);
                    i32 rr = clamp_i((i32)((m.y - pmin.y) / (sheet.y / (f32)fy)), 0, fy - 1);
                    editor->preview.frame = rr * fx + cc;
                }
            }
        }
    }
    else
    {
        ImGui::TextDisabled("(select an image on the left)");
    }

    ImGui::End();
    ImGui::PopStyleColor();
}

} // namespace Editor
