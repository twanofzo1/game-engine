/**
 * @file options_panel.cpp
 * @brief Dear ImGui "Options" panel, switching between sprite and animation editing.
 */
#include "ui/panels.hpp"
#include "editors/animations.hpp"
#include "services/dnd.hpp"
#include "services/guard.hpp"

#include "imgui.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

using namespace Engine;

namespace Editor {

/** @brief Clamp `v` to the inclusive range [lo, hi]. */
static i32 clamp_i(i32 v, i32 lo, i32 hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

/** @brief Draw the sprite-editing controls. */
static void draw_sprite_options(Editor* editor)
{
    /* Texture slot: drop an image from Assets to change the preview. */
    char tex[EDITOR_MAX_PATH];
    snprintf(tex, sizeof(tex), "%s", editor->preview.preview_path);
    if (dnd_slot_string("texture", DND_IMAGE_PATH,
                        editor->preview.preview_path[0] ? editor->preview.preview_path : "(none)",
                        tex, EDITOR_MAX_PATH))
        request_open_image(editor, tex);

    if (ImGui::InputText("name", editor->sprites.sprite_name, EDITOR_SPRITE_NAME_MAX))
        mark_dirty(editor);

    if (ImGui::InputInt("h frames", &editor->preview.frames_x))
    {
        editor->preview.frames_x = clamp_i(editor->preview.frames_x, 1, 256);
        mark_dirty(editor);
    }
    if (ImGui::InputInt("v frames", &editor->preview.frames_y))
    {
        editor->preview.frames_y = clamp_i(editor->preview.frames_y, 1, 256);
        mark_dirty(editor);
    }

    i32 total = editor->preview.frames_x * editor->preview.frames_y;
    editor->preview.frame = clamp_i(editor->preview.frame, 0, total - 1);

    ImGui::Separator();

    if (ImGui::SliderInt("frame", &editor->preview.frame, 0, total - 1))
        editor->preview.frame = clamp_i(editor->preview.frame, 0, total - 1);

    if (ImGui::Button("<"))
        editor->preview.frame = (editor->preview.frame + total - 1) % total;
    ImGui::SameLine();
    if (ImGui::Button(">"))
        editor->preview.frame = (editor->preview.frame + 1) % total;
    ImGui::SameLine();
    ImGui::Text("%d / %d", editor->preview.frame, total);

    ImGui::Separator();

    ImGui::SliderFloat("zoom", &editor->preview.zoom, 0.1f, 8.0f, "%.1fx");
    if (ImGui::Button("Fit"))
        editor->preview.zoom = 1.0f;
    ImGui::Checkbox("single frame", &editor->preview.single_frame);

    ImGui::Separator();

    if (ImGui::Button("Save as sprite", ImVec2(-1.0f, 0.0f)))
        save_sprite(editor, editor->config_path);
}

/** @brief Draw the sprite selector combo; return that sprite's frame count. */
static i32 draw_sprite_combo(Editor* editor)
{
    char sprite_buf[EDITOR_SPRITE_NAME_MAX + EDITOR_MAX_PATH + 16];
    const char* sprite_label = "(none)";
    i32 total_frames = 0;
    for (i32 s = 0; s < editor->sprites.sprite_count; ++s)
    {
        if (editor->sprites.sprites[s].yaml_index == editor->animation.anim_sprite_id)
        {
            const SpriteEntry* sp = &editor->sprites.sprites[s];
            snprintf(sprite_buf, sizeof(sprite_buf), "%s",
                     sp->name[0] ? sp->name : sp->path);
            sprite_label = sprite_buf;
            total_frames = sp->frames_x * sp->frames_y;
        }
    }
    if (dnd_slot_i32("sprite", DND_SPRITE, sprite_label, &editor->animation.anim_sprite_id))
        mark_dirty(editor);

    if (ImGui::BeginCombo("##sprite_pick", sprite_label))
    {
        for (i32 s = 0; s < editor->sprites.sprite_count; ++s)
        {
            const SpriteEntry* sp = &editor->sprites.sprites[s];
            char item[EDITOR_SPRITE_NAME_MAX + EDITOR_MAX_PATH + 16];
            snprintf(item, sizeof(item), "%s###spr%d",
                     sp->name[0] ? sp->name : sp->path, s);
            if (ImGui::Selectable(item, editor->animation.anim_sprite_id == sp->yaml_index))
            {
                editor->animation.anim_sprite_id = sp->yaml_index;
                mark_dirty(editor);
            }
        }
        ImGui::EndCombo();
    }
    return total_frames;
}

/** @brief Draw the animation-editing controls. */
static void draw_animation_options(Editor* editor)
{
    if (ImGui::InputText("name", editor->animation.anim_name, EDITOR_ANIMATION_NAME_MAX))
        mark_dirty(editor);
    i32 total_frames = draw_sprite_combo(editor);

    ImGui::Separator();

    ImGui::InputInt("frame", &editor->animation.anim_frame);
    if (total_frames > 0)
        ImGui::TextDisabled("frame range: 0..%d", total_frames - 1);
    ImGui::InputFloat("time", &editor->animation.anim_time, 0.01f, 0.1f, "%.2f");
    ImGui::InputFloat("time step", &editor->animation.anim_time_step, 0.01f, 0.1f, "%.2f");
    if (ImGui::Button("Add keyframe"))
    {
        animation_add_keyframe(editor, editor->animation.anim_time, editor->animation.anim_frame);
        editor->animation.anim_time += editor->animation.anim_time_step;
        editor->animation.anim_frame++;
        mark_dirty(editor);
    }

    ImGui::Separator();

    ImGui::Text("keyframes (%d)", editor->animation.anim_keyframe_count);
    for (i32 k = 0; k < editor->animation.anim_keyframe_count; ++k)
    {
        ImGui::PushID(k);
        ImGui::Text("%.2fs -> frame %d",
                    editor->animation.anim_keyframes[k].time,
                    editor->animation.anim_keyframes[k].frame);
        ImGui::SameLine();
        if (ImGui::SmallButton("x"))
        {
            animation_remove_keyframe(editor, k);
            mark_dirty(editor);
        }
        ImGui::PopID();
    }

    ImGui::Separator();

    ImGui::SliderFloat("speed", &editor->animation.anim_speed, 0.1f, 4.0f, "%.2fx");
    if (ImGui::Button("Save animation", ImVec2(-1.0f, 0.0f)))
        save_animation(editor, editor->config_path);
    if (ImGui::Button("New animation", ImVec2(-1.0f, 0.0f)))
        animation_clear(editor);
}

void draw_options(Editor* editor)
{
    ImGui::Begin("Options");

    /* Shared preview filter control, visible in both modes. */
    if (editor->preview.preview_loaded)
    {
        if (ImGui::Checkbox("nearest", &editor->preview.nearest))
        {
            Texture::Settings s = Texture::settings_default();
            if (editor->preview.nearest)
            {
                s.min_filter = Texture::FILTER_NEAREST;
                s.mag_filter = Texture::FILTER_NEAREST;
            }
            Texture::set_settings(&editor->preview.gpu, &s);
            mark_dirty(editor);
        }
    }
    ImGui::Text("filter: %s", editor->preview.nearest ? "nearest" : "linear");
    ImGui::Separator();

    if (editor->state == STATE_ANIMATION)
    {
        ImGui::Text("animation editor");
        ImGui::Separator();
        draw_animation_options(editor);
    }
    else if (editor->state == STATE_ATTACK)
    {
        ImGui::Text("attack editor");
        ImGui::Separator();
        draw_attack_options(editor);
    }
    else
    {
        ImGui::Text("sprite editor");
        ImGui::Separator();

        if (!editor->preview.preview_loaded)
        {
            ImGui::TextDisabled("no image loaded");
            ImGui::End();
            return;
        }

        draw_sprite_options(editor);

        /* Delete key deletes the currently selected sprite (when not typing). */
        if (editor->sprites.selected_sprite >= 0 && !ImGui::IsAnyItemActive() &&
            ImGui::IsKeyPressed(ImGuiKey_Delete, false))
            delete_sprite_at(editor, editor->config_path,
                             editor->sprites.sprites[editor->sprites.selected_sprite].yaml_index);
    }

    ImGui::Separator();
    ImGui::Text("size: %d x %d", editor->preview.gpu.width, editor->preview.gpu.height);
    ImGui::Text("path: %s", editor->preview.preview_path);

    ImGui::End();
}

} // namespace Editor
