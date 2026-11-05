/**
 * @file attack_options.cpp
 * @brief Dear ImGui attack-editing controls: phases, events, hitboxes, movement.
 */
#include "ui/panels.hpp"
#include "editors/attacks.hpp"
#include "services/dnd.hpp"
#include "editors/shapes.hpp"
#include "services/guard.hpp"
#include "services/undo.hpp"

#include "imgui.h"

#include <stdio.h>

using namespace Engine;

namespace Editor {

/** @brief Slot + combo selecting the attack's animation by yaml index. */
static void draw_animation_combo(Editor* editor)
{
    const char* label = "(none)";
    for (i32 a = 0; a < editor->animation.animation_count; ++a)
        if (editor->animation.animations[a].yaml_index == editor->attack.attack_animation_id)
            label = editor->animation.animations[a].name;

    if (dnd_slot_i32("animation", DND_ANIMATION, label, &editor->attack.attack_animation_id))
        mark_dirty(editor);

    if (ImGui::BeginCombo("##animation_pick", label))
    {
        for (i32 a = 0; a < editor->animation.animation_count; ++a)
        {
            const AnimationEntry* anim = &editor->animation.animations[a];
            char item[EDITOR_ANIMATION_NAME_MAX + 16];
            snprintf(item, sizeof(item), "%s###anim%d", anim->name, a);
            if (ImGui::Selectable(item, editor->attack.attack_animation_id == anim->yaml_index))
            {
                editor->attack.attack_animation_id = anim->yaml_index;
                mark_dirty(editor);
            }
        }
        ImGui::EndCombo();
    }
}

/** @brief Slot + combo selecting a collision shape by yaml index. */
static void draw_shape_combo(Editor* editor, i32* shape_id)
{
    char buf[64];
    const char* label = "(none)";
    if (*shape_id >= 0)
    {
        snprintf(buf, sizeof(buf), "shape %d", *shape_id);
        label = buf;
    }

    if (dnd_slot_i32("shape", DND_SHAPE, label, shape_id))
        mark_dirty(editor);

    if (ImGui::BeginCombo("##shape_pick", label))
    {
        if (ImGui::Selectable("(none)", *shape_id < 0))
            *shape_id = -1;
        for (i32 s = 0; s < editor->shapes.shape_count; ++s)
        {
            const ShapeEntry* sh = &editor->shapes.shapes[s];
            const char* kind = sh->type == 1 ? "rect" : (sh->type == 2 ? "poly" : "circle");
            char item[64];
            snprintf(item, sizeof(item), "%d: %s###shape%d", sh->yaml_index, kind, s);
            if (ImGui::Selectable(item, *shape_id == sh->yaml_index))
                *shape_id = sh->yaml_index;
        }
        ImGui::EndCombo();
    }
}

/** @brief Event keyframe list + add-at-playhead. */
static void draw_event_editor(Editor* editor)
{
    ImGui::Text("events (%d)", editor->attack.attack_event_count);
    for (i32 e = 0; e < editor->attack.attack_event_count; ++e)
    {
        ImGui::PushID(e);
        ImGui::Text("frame %d -> %s", editor->attack.attack_events[e].frame,
                    editor->attack.attack_events[e].name);
        ImGui::SameLine();
        if (ImGui::SmallButton("x"))
        {
            attack_remove_event(editor, e);
            mark_dirty(editor);
        }
        ImGui::PopID();
    }

    ImGui::InputText("event name", editor->attack.attack_event_name, EDITOR_EVENT_NAME_MAX);
    if (ImGui::Button("Add event at playhead"))
    {
        attack_add_event(editor, editor->attack.attack_play_frame, editor->attack.attack_event_name);
        mark_dirty(editor);
    }
}

/** @brief Hitbox keyframe list + add-at-playhead. */
static void draw_hitbox_editor(Editor* editor)
{
    ImGui::Text("hitboxes (%d)", editor->attack.attack_hitbox_count);
    for (i32 h = 0; h < editor->attack.attack_hitbox_count; ++h)
    {
        ImGui::PushID(h);
        if (editor->attack.attack_hitboxes[h].shape_id < 0)
            ImGui::Text("frame %d -> none", editor->attack.attack_hitboxes[h].frame);
        else
            ImGui::Text("frame %d -> shape %d", editor->attack.attack_hitboxes[h].frame,
                        editor->attack.attack_hitboxes[h].shape_id);
        ImGui::SameLine();
        if (ImGui::SmallButton("x"))
        {
            attack_remove_hitbox(editor, h);
            mark_dirty(editor);
        }
        ImGui::PopID();
    }

    draw_shape_combo(editor, &editor->attack.attack_hitbox_shape);
    if (ImGui::Button("Set hitbox at playhead"))
    {
        undo_push(editor);
        editor->guard.dirty = true;
        attack_set_hitbox(editor, editor->attack.attack_play_frame, editor->attack.attack_hitbox_shape);
    }
}

/** @brief Movement keyframe list + vector edit at the playhead. */
static void draw_movement_editor(Editor* editor)
{
    ImGui::Text("movement (%d)", editor->attack.attack_movement_count);
    for (i32 m = 0; m < editor->attack.attack_movement_count; ++m)
    {
        ImGui::PushID(m);
        ImGui::Text("frame %d -> (%.0f, %.0f)", editor->attack.attack_movements[m].frame,
                    editor->attack.attack_movements[m].vector.x,
                    editor->attack.attack_movements[m].vector.y);
        ImGui::SameLine();
        if (ImGui::SmallButton("x"))
        {
            attack_remove_movement(editor, m);
            mark_dirty(editor);
        }
        ImGui::PopID();
    }

    if (ImGui::InputFloat2("vector", &editor->attack.attack_move.x))
        mark_dirty(editor);
    if (ImGui::Button("Add movement at playhead"))
    {
        attack_add_movement(editor, editor->attack.attack_play_frame, editor->attack.attack_move);
        mark_dirty(editor);
    }
}

/** @brief Collision-shape editor (the hitbox editor). */
static void draw_shape_editor(Editor* editor)
{
    ImGui::Text("shapes (%d)", editor->shapes.shape_count);
    for (i32 s = 0; s < editor->shapes.shape_count; ++s)
    {
        ShapeEntry* sh = &editor->shapes.shapes[s];
        const char* kind = sh->type == 1 ? "rect" : (sh->type == 2 ? "polygon" : "circle");
        char label[64];
        snprintf(label, sizeof(label), "%d: %s###shape%d", sh->yaml_index, kind, s);
        if (ImGui::Selectable(label, editor->shapes.selected_shape == s))
            shape_select(editor, s);
        dnd_source_i32(DND_SHAPE, sh->yaml_index, label);
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
                delete_shape_at(editor, editor->config_path, sh->yaml_index);
            ImGui::EndPopup();
        }
    }

    ImGui::Separator();

    if (ImGui::Button("New shape"))
    {
        shape_new(editor);
        mark_dirty(editor);
    }

    if (editor->shapes.selected_shape < 0 || editor->shapes.selected_shape >= editor->shapes.shape_count)
        return;

    ShapeEntry* sh = &editor->shapes.shapes[editor->shapes.selected_shape];

    ImGui::Separator();
    const char* types[] = {"circle", "rect", "polygon"};
    if (ImGui::Combo("type", &sh->type, types, 3))
        mark_dirty(editor);

    if (sh->type == 1)
    {
        if (ImGui::DragFloat2("position", &sh->pos.x, 1.0f)) mark_dirty(editor);
        if (ImGui::DragFloat2("size", &sh->size.x, 1.0f)) mark_dirty(editor);
        if (ImGui::DragFloat("rotation", &sh->rotation, 0.01f)) mark_dirty(editor);
    }
    else if (sh->type == 2)
    {
        ImGui::Text("points (%d)", sh->point_count);
        for (i32 p = 0; p < sh->point_count; ++p)
        {
            ImGui::PushID(p);
            if (ImGui::DragFloat2("##pt", &sh->points[p].x, 1.0f)) mark_dirty(editor);
            ImGui::SameLine();
            if (ImGui::SmallButton("x"))
            {
                for (i32 q = p; q + 1 < sh->point_count; ++q)
                    sh->points[q] = sh->points[q + 1];
                sh->point_count--;
                mark_dirty(editor);
            }
            ImGui::PopID();
        }
        if (ImGui::Button("Add point") && sh->point_count < EDITOR_SHAPE_MAX_POINTS)
        {
            sh->points[sh->point_count] = Vec2::make(0.0f, 0.0f);
            sh->point_count++;
            mark_dirty(editor);
        }
    }
    else
    {
        if (ImGui::DragFloat2("center", &sh->center.x, 1.0f)) mark_dirty(editor);
        if (ImGui::DragFloat("radius", &sh->radius, 1.0f, 0.0f, 4096.0f)) mark_dirty(editor);
    }

    if (ImGui::Button("Save shapes", ImVec2(-1.0f, 0.0f)))
        save_shapes(editor, editor->config_path);
}

void draw_attack_options(Editor* editor)
{
    if (ImGui::InputText("name", editor->attack.attack_name, EDITOR_SPRITE_NAME_MAX))
        mark_dirty(editor);
    draw_animation_combo(editor);

    ImGui::Separator();

    if (ImGui::DragInt("windup", &editor->attack.attack_windup, 1, 0, 1000))
        mark_dirty(editor);
    if (ImGui::DragInt("active", &editor->attack.attack_active, 1, 0, 1000))
        mark_dirty(editor);
    if (ImGui::DragInt("recovery", &editor->attack.attack_recovery, 1, 0, 1000))
        mark_dirty(editor);

    ImGui::Separator();

    draw_event_editor(editor);
    ImGui::Separator();
    draw_hitbox_editor(editor);
    ImGui::Separator();
    draw_movement_editor(editor);
    ImGui::Separator();
    draw_shape_editor(editor);

    ImGui::Separator();

    if (ImGui::Button("Save attack", ImVec2(-1.0f, 0.0f)))
        save_attack(editor, editor->config_path);
    if (ImGui::Button("New attack", ImVec2(-1.0f, 0.0f)))
        attack_clear(editor);
}

} // namespace Editor
