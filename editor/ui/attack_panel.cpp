/**
 * @file attack_panel.cpp
 * @brief Dear ImGui panel for the attack editor: timeline with animation,
 *        phases, events, hitbox markers and movement vectors.
 */
#include "ui/panels.hpp"
#include "editors/attacks.hpp"
#include "services/guard.hpp"

#include "imgui.h"

#include <math.h>
#include <stdio.h>

using namespace Engine;

namespace Editor {

/** @brief Clamp `v` to the inclusive range [lo, hi]. */
static f32 clampf(f32 v, f32 lo, f32 hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

/** @brief The animation whose yaml index equals `animation_id`, or NULL. */
static const AnimationEntry* find_animation(const Editor* editor, i32 animation_id)
{
    for (i32 a = 0; a < editor->animation.animation_count; ++a)
        if (editor->animation.animations[a].yaml_index == animation_id)
            return &editor->animation.animations[a];
    return NULL;
}

/** @brief Draw the attack timeline graph with toggleable movement/hitbox/frame lanes. */
static void draw_attack_timeline(Editor* editor, const ImVec2& avail)
{
    i32 total = attack_total_duration(editor);
    f32 h = avail.y < 20.0f ? 20.0f : avail.y;

    ImGui::InvisibleButton("##attack_timeline", ImVec2(avail.x, h));
    ImVec2 pmin = ImGui::GetItemRectMin();
    ImVec2 pmax = ImGui::GetItemRectMax();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRectFilled(pmin, pmax, IM_COL32(24, 24, 32, 255));

    f32 span = pmax.x - pmin.x;
    if (span <= 1.0f)
        return;

    /* Frame -> x mapping. */
    auto x_at = [&](f32 f) { return pmin.x + span * clampf(f / (f32)total, 0.0f, 1.0f); };

    /* Phase shading: windup (blue), active (red), recovery (green). */
    i32 w_end = editor->attack.attack_windup;
    i32 a_end = editor->attack.attack_windup + editor->attack.attack_active;
    dl->AddRectFilled(pmin, ImVec2(x_at((f32)w_end), pmax.y), IM_COL32(40, 60, 120, 55));
    dl->AddRectFilled(ImVec2(x_at((f32)w_end), pmin.y), ImVec2(x_at((f32)a_end), pmax.y),
                      IM_COL32(120, 50, 50, 55));
    dl->AddRectFilled(ImVec2(x_at((f32)a_end), pmin.y), pmax, IM_COL32(50, 110, 60, 55));

    /* Count visible lanes and split the height between them. */
    i32 lanes = 0;
    if (editor->attack.timeline_show_movement) lanes++;
    if (editor->attack.timeline_show_hitbox) lanes++;
    if (editor->attack.timeline_show_frame) lanes++;
    if (lanes == 0) lanes = 1;

    f32 lane_h = (pmax.y - pmin.y) / (f32)lanes;
    f32 lane_i = 0.0f;

    const AnimationEntry* anim = find_animation(editor, editor->attack.attack_animation_id);

    /* Movement lane: vector arrows at each keyframe. */
    if (editor->attack.timeline_show_movement)
    {
        f32 y0 = pmin.y + lane_h * lane_i;
        f32 y1 = y0 + lane_h;
        lane_i += 1.0f;
        f32 mid = (y0 + y1) * 0.5f;
        dl->AddText(ImVec2(pmin.x + 3.0f, y0 + 2.0f), IM_COL32(150, 200, 255, 210), "movement");
        for (i32 m = 0; m < editor->attack.attack_movement_count; ++m)
        {
            f32 x = x_at((f32)editor->attack.attack_movements[m].frame);
            Vec2::Vec2 v = editor->attack.attack_movements[m].vector;
            f32 len = sqrtf(v.x * v.x + v.y * v.y);
            f32 s = 14.0f / (len > 0.001f ? len : 1.0f);
            ImVec2 tip(x + v.x * s, mid - v.y * s);
            dl->AddLine(ImVec2(x, mid), tip, IM_COL32(90, 200, 255, 230), 2.0f);
            dl->AddCircleFilled(tip, 3.0f, IM_COL32(90, 200, 255, 230), 0);
        }
    }

    /* Hitbox lane: a per-frame bar for each active shape interval. */
    if (editor->attack.timeline_show_hitbox)
    {
        f32 y0 = pmin.y + lane_h * lane_i;
        f32 y1 = y0 + lane_h;
        lane_i += 1.0f;
        dl->AddText(ImVec2(pmin.x + 3.0f, y0 + 2.0f), IM_COL32(255, 180, 100, 210), "hitbox");
        for (i32 hb = 0; hb < editor->attack.attack_hitbox_count; ++hb)
        {
            i32 f0 = editor->attack.attack_hitboxes[hb].frame;
            i32 f1 = (hb + 1 < editor->attack.attack_hitbox_count)
                         ? editor->attack.attack_hitboxes[hb + 1].frame
                         : total;
            f32 x0 = x_at((f32)f0);
            f32 x1 = x_at((f32)f1);
            if (x1 <= x0) x1 = x0 + 2.0f;

            i32 sid = editor->attack.attack_hitboxes[hb].shape_id;
            if (sid < 0)
            {
                dl->AddRectFilled(ImVec2(x0, y0 + 8.0f), ImVec2(x1, y1 - 4.0f),
                                  IM_COL32(90, 90, 100, 60));
                dl->AddRect(ImVec2(x0, y0 + 8.0f), ImVec2(x1, y1 - 4.0f),
                            IM_COL32(140, 140, 150, 220), 0.0f, 0, 1.0f);
                dl->AddText(ImVec2(x0 + 3.0f, y0 + 10.0f),
                            IM_COL32(180, 180, 190, 230), "none");
            }
            else
            {
                dl->AddRectFilled(ImVec2(x0, y0 + 8.0f), ImVec2(x1, y1 - 4.0f),
                                  IM_COL32(255, 150, 60, 60));
                dl->AddRect(ImVec2(x0, y0 + 8.0f), ImVec2(x1, y1 - 4.0f),
                            IM_COL32(255, 150, 60, 220), 0.0f, 0, 1.0f);
                char buf[24];
                snprintf(buf, sizeof(buf), "hb %d", sid);
                dl->AddText(ImVec2(x0 + 3.0f, y0 + 10.0f),
                            IM_COL32(255, 200, 140, 230), buf);
            }
        }
    }

    /* Frame lane: the animation's sprite frames, one per attack frame index. */
    if (editor->attack.timeline_show_frame)
    {
        f32 y0 = pmin.y + lane_h * lane_i;
        f32 y1 = y0 + lane_h;
        lane_i += 1.0f;
        dl->AddText(ImVec2(pmin.x + 3.0f, y0 + 2.0f), IM_COL32(220, 220, 220, 210), "frame");
        if (anim)
        {
            for (i32 k = 0; k < anim->keyframe_count; ++k)
            {
                f32 x = x_at((f32)k);
                dl->AddLine(ImVec2(x, y0 + 6.0f), ImVec2(x, y1 - 2.0f),
                            IM_COL32(220, 220, 220, 140), 1.0f);
                char buf[16];
                snprintf(buf, sizeof(buf), "%d", anim->keyframes[k].frame);
                dl->AddText(ImVec2(x + 2.0f, y0 + 2.0f), IM_COL32(220, 220, 220, 220), buf);
            }
        }
    }

    /* Event markers: full-height lines with the event name at the top. */
    for (i32 e = 0; e < editor->attack.attack_event_count; ++e)
    {
        f32 x = x_at((f32)editor->attack.attack_events[e].frame);
        dl->AddLine(ImVec2(x, pmin.y), ImVec2(x, pmax.y), IM_COL32(255, 255, 255, 140), 1.0f);
        dl->AddText(ImVec2(x + 3.0f, pmin.y + 2.0f), IM_COL32(255, 255, 255, 220),
                    editor->attack.attack_events[e].name);
    }

    /* Playhead. */
    f32 px = x_at((f32)editor->attack.attack_play_frame);
    dl->AddLine(ImVec2(px, pmin.y), ImVec2(px, pmax.y), IM_COL32(255, 210, 0, 255), 2.0f);

    /* Scrub. */
    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
    {
        ImVec2 m = ImGui::GetMousePos();
        editor->attack.attack_play_frame = (i32)(((m.x - pmin.x) / span) * (f32)total);
        if (editor->attack.attack_play_frame < 0) editor->attack.attack_play_frame = 0;
        if (editor->attack.attack_play_frame >= total) editor->attack.attack_play_frame = total - 1;
        editor->preview.frame = attack_frame_at(editor, editor->attack.attack_play_frame);
    }
}

void draw_attacks(Editor* editor)
{
    attack_tick(editor, ImGui::GetIO().DeltaTime);

    ImGui::Begin("Attacks");

    if (ImGui::Button(editor->attack.attack_playing ? "Pause" : "Play"))
        editor->attack.attack_playing = !editor->attack.attack_playing;
    ImGui::SameLine();
    ImGui::Text("frame=%d  sprite=%d", editor->attack.attack_play_frame, editor->preview.frame);

    ImGui::Checkbox("movement", &editor->attack.timeline_show_movement);
    ImGui::SameLine();
    ImGui::Checkbox("hitbox", &editor->attack.timeline_show_hitbox);
    ImGui::SameLine();
    ImGui::Checkbox("frame", &editor->attack.timeline_show_frame);

    ImGui::BeginChild("##attack_timeline", ImVec2(0.0f, 108.0f), false);
    draw_attack_timeline(editor, ImGui::GetContentRegionAvail());
    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("New attack", ImVec2(-1.0f, 0.0f)))
        request_new_attack(editor);

    ImGui::End();
}

} // namespace Editor
