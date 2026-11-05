/**
 * @file anim_panel.cpp
 * @brief Dear ImGui panel for the animation editor: preview, timeline, controls.
 */
#include "ui/panels.hpp"
#include "editors/animations.hpp"
#include "services/guard.hpp"

#include "imgui.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

using namespace Engine;

namespace Editor {


/** @brief Draw the timeline strip (keyframe markers + playhead) and scrub. */
static void draw_timeline(Editor* editor, const ImVec2& avail)
{
    f32 duration = animation_duration(editor);
    f32 h = avail.y < 20.0f ? 20.0f : avail.y;

    ImGui::InvisibleButton("##timeline", ImVec2(avail.x, h));
    ImVec2 pmin = ImGui::GetItemRectMin();
    ImVec2 pmax = ImGui::GetItemRectMax();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRectFilled(pmin, pmax, IM_COL32(35, 35, 45, 255));

    f32 span = pmax.x - pmin.x;
    if (span > 1.0f)
    {
        for (i32 k = 0; k < editor->animation.anim_keyframe_count; ++k)
        {
            f32 x = pmin.x + span * (editor->animation.anim_keyframes[k].time / duration);
            dl->AddLine(ImVec2(x, pmin.y), ImVec2(x, pmax.y),
                        IM_COL32(255, 255, 255, 110), 1.0f);
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", editor->animation.anim_keyframes[k].frame);
            dl->AddText(ImVec2(x + 3.0f, pmin.y + 2.0f),
                        IM_COL32(255, 255, 255, 220), buf);
        }

        f32 x = pmin.x + span * (editor->animation.anim_play_time / duration);
        dl->AddLine(ImVec2(x, pmin.y), ImVec2(x, pmax.y),
                    IM_COL32(255, 210, 0, 255), 2.0f);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
        {
            ImVec2 m = ImGui::GetMousePos();
            editor->animation.anim_play_time = ((m.x - pmin.x) / span) * duration;
            editor->preview.frame = animation_frame_at(editor, editor->animation.anim_play_time);
        }
    }
}

void draw_animations(Editor* editor)
{
    animation_tick(editor, ImGui::GetIO().DeltaTime);

    ImGui::Begin("Animations");

    /* Playback controls + timeline. */
    if (ImGui::Button(editor->animation.anim_playing ? "Pause" : "Play"))
        editor->animation.anim_playing = !editor->animation.anim_playing;
    ImGui::SameLine();
    ImGui::Text("t=%.2fs  frame=%d", editor->animation.anim_play_time, editor->preview.frame);

    ImGui::BeginChild("##anim_timeline", ImVec2(0.0f, 48.0f), false);
    draw_timeline(editor, ImGui::GetContentRegionAvail());
    ImGui::EndChild();

    ImGui::Separator();

    /* New animation (bottom). */
    if (ImGui::Button("New animation", ImVec2(-1.0f, 0.0f)))
        request_new_animation(editor);

    ImGui::End();
}

} // namespace Editor
