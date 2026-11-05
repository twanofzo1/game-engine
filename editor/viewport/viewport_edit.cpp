/**
 * @file viewport_edit.cpp
 * @brief Direct manipulation of hitboxes and movement in the viewport.
 *
 * While editing an attack, the viewport draws the active hitbox shape with
 * draggable handles (circle center/radius, rect body/corners, polygon points)
 * and the movement vector with a draggable tip. Edits write directly into the
 * editor's shape and movement lists and mark them dirty.
 */
#include "viewport/viewport_edit.hpp"
#include "editors/attacks.hpp"
#include "services/guard.hpp"
#include "editors/shapes.hpp"
#include "services/undo.hpp"

#include "imgui.h"

#include <math.h>

using namespace Engine;

namespace Editor {

/** @brief Index into `shapes` whose yaml index equals `shape_id`, or -1. */
static i32 find_shape(const Editor* editor, i32 shape_id)
{
    for (i32 s = 0; s < editor->shapes.shape_count; ++s)
        if (editor->shapes.shapes[s].yaml_index == shape_id)
            return s;
    return -1;
}

/** @brief True when `a` is within `r` pixels of `b`. */
static bool near(ImVec2 a, ImVec2 b, f32 r)
{
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return (dx * dx + dy * dy) <= r * r;
}

/** @brief World (config) coordinates of a screen point. */
static Vec2::Vec2 world_at(const ImVec2& center, f32 scale, const ImVec2& mouse)
{
    return Vec2::make((mouse.x - center.x) / scale, (center.y - mouse.y) / scale);
}

/** @brief Screen position of a world point. */
static ImVec2 screen_at(const ImVec2& center, f32 scale, Vec2::Vec2 w)
{
    return ImVec2(center.x + w.x * scale, center.y - w.y * scale);
}

/** @brief True when a hitbox keyframe already exists at `frame`. */
static bool has_hitbox_at(const Editor* editor, i32 frame)
{
    for (i32 h = 0; h < editor->attack.attack_hitbox_count; ++h)
        if (editor->attack.attack_hitboxes[h].frame == frame)
            return true;
    return false;
}

/** @brief True when `shape_id` is referenced by more than one hitbox keyframe. */
static bool shape_is_shared(const Editor* editor, i32 shape_id)
{
    i32 count = 0;
    for (i32 h = 0; h < editor->attack.attack_hitbox_count; ++h)
        if (editor->attack.attack_hitboxes[h].shape_id == shape_id)
            count++;
    return count > 1;
}

/** @brief Begin a shape edit: snapshot, duplicate if needed, bind at playhead. */
static void begin_shape_edit(Editor* editor, i32* si, i32 drag_code, i32 sub)
{
    undo_push(editor);
    editor->guard.dirty = true;

    i32 shape_id = editor->shapes.shapes[*si].yaml_index;
    bool duplicate = !has_hitbox_at(editor, editor->attack.attack_play_frame) ||
                     shape_is_shared(editor, shape_id);

    if (duplicate)
    {
        i32 new_id = shape_duplicate(editor, *si);
        if (new_id >= 0)
        {
            attack_set_hitbox(editor, editor->attack.attack_play_frame, new_id);
            *si = find_shape(editor, new_id);
        }
        else
        {
            attack_set_hitbox(editor, editor->attack.attack_play_frame, shape_id);
        }
    }
    else
    {
        attack_set_hitbox(editor, editor->attack.attack_play_frame, shape_id);
    }

    shape_select(editor, *si);
    editor->viewport.view_edit_drag = drag_code;
    editor->viewport.view_edit_drag_sub = sub;
}

/** @brief Begin a movement edit: snapshot and dirty. */
static void begin_movement_edit(Editor* editor)
{
    undo_push(editor);
    editor->guard.dirty = true;
    editor->viewport.view_edit_drag = 6;
    editor->viewport.view_edit_drag_sub = 0;
}

/** @brief Draw + edit the active shape. */
static void edit_shape(Editor* editor, ImDrawList* dl, i32* si,
                       const ImVec2& center, f32 scale)
{
    ShapeEntry* sh = &editor->shapes.shapes[*si];
    ImVec2 mouse = ImGui::GetMousePos();
    bool clicked = ImGui::IsMouseClicked(0);
    bool down = ImGui::IsMouseDown(0);

    ImU32 hl = IM_COL32(255, 220, 0, 255);

    if (sh->type == 0) /* circle */
    {
        ImVec2 c = screen_at(center, scale, sh->center);
        ImVec2 rp = ImVec2(c.x + sh->radius * scale, c.y);
        dl->AddCircleFilled(c, 5.0f, hl, 0);
        dl->AddCircleFilled(rp, 5.0f, hl, 0);

        if (clicked && near(mouse, rp, 8.0f))
        {
            begin_shape_edit(editor, si, 2, 0);
        }
        else if (clicked && (near(mouse, c, 8.0f) ||
                             near(mouse, c, sh->radius * scale)))
        {
            begin_shape_edit(editor, si, 1, 0);
        }
        sh = &editor->shapes.shapes[*si];

        if (editor->viewport.view_edit_drag == 2 && down)
        {
            sh->radius = (mouse.x - c.x) / scale;
            if (sh->radius < 1.0f) sh->radius = 1.0f;
            mark_dirty(editor);
        }
        else if (editor->viewport.view_edit_drag == 1 && down)
        {
            sh->center = world_at(center, scale, mouse);
            mark_dirty(editor);
        }
    }
    else if (sh->type == 1) /* rect */
    {
        f32 x0 = center.x + sh->pos.x * scale;
        f32 x1 = center.x + (sh->pos.x + sh->size.x) * scale;
        f32 y0 = center.y - sh->pos.y * scale;                 /* bottom */
        f32 y1 = center.y - (sh->pos.y + sh->size.y) * scale;  /* top */
        ImVec2 p[4] = { ImVec2(x0, y1), ImVec2(x1, y1),
                        ImVec2(x0, y0), ImVec2(x1, y0) };      /* TL TR BL BR */

        for (i32 k = 0; k < 4; ++k)
            dl->AddCircleFilled(p[k], 5.0f, hl, 0);

        if (clicked)
        {
            for (i32 k = 0; k < 4; ++k)
            {
                if (near(mouse, p[k], 8.0f))
                {
                    begin_shape_edit(editor, si, 4, k);
                    break;
                }
            }
            if (editor->viewport.view_edit_drag == 0 && mouse.x >= x0 && mouse.x <= x1 &&
                mouse.y >= y1 && mouse.y <= y0)
            {
                begin_shape_edit(editor, si, 3, 0);
            }
        }
        sh = &editor->shapes.shapes[*si];

        if (editor->viewport.view_edit_drag == 3 && down)
        {
            Vec2::Vec2 w = world_at(center, scale, mouse);
            sh->pos.x = w.x - sh->size.x * 0.5f;
            sh->pos.y = w.y - sh->size.y * 0.5f;
            mark_dirty(editor);
        }
        else if (editor->viewport.view_edit_drag == 4 && down)
        {
            Vec2::Vec2 w = world_at(center, scale, mouse);
            f32 px = sh->pos.x, py = sh->pos.y, sx = sh->size.x, sy = sh->size.y;
            f32 right = px + sx, top = py + sy;
            switch (editor->viewport.view_edit_drag_sub)
            {
                case 0: sh->pos.x = w.x; sh->size.x = right - w.x; sh->size.y = w.y - py; break;
                case 1: sh->size.x = w.x - px; sh->size.y = w.y - py; break;
                case 2: sh->pos.x = w.x; sh->size.x = right - w.x; sh->pos.y = w.y; sh->size.y = top - w.y; break;
                default: sh->size.x = w.x - px; sh->pos.y = w.y; sh->size.y = top - w.y; break;
            }
            if (sh->size.x < 1.0f) sh->size.x = 1.0f;
            if (sh->size.y < 1.0f) sh->size.y = 1.0f;
            mark_dirty(editor);
        }
    }
    else /* polygon */
    {
        for (i32 k = 0; k < sh->point_count; ++k)
        {
            ImVec2 p = screen_at(center, scale, sh->points[k]);
            dl->AddCircleFilled(p, 5.0f, hl, 0);
            if (clicked && near(mouse, p, 8.0f))
            {
                begin_shape_edit(editor, si, 5, k);
            }
        }
        sh = &editor->shapes.shapes[*si];

        if (editor->viewport.view_edit_drag == 5 && down && editor->viewport.view_edit_drag_sub >= 0 &&
            editor->viewport.view_edit_drag_sub < sh->point_count)
        {
            sh->points[editor->viewport.view_edit_drag_sub] = world_at(center, scale, mouse);
            mark_dirty(editor);
        }
    }
}

/** @brief Draw + edit the movement vector arrow. */
static void edit_movement(Editor* editor, ImDrawList* dl,
                          const ImVec2& center, f32 scale)
{
    Vec2::Vec2 mv = attack_movement_at(editor, editor->attack.attack_play_frame);
    ImVec2 tip = screen_at(center, scale, mv);
    ImVec2 mouse = ImGui::GetMousePos();

    ImU32 col = IM_COL32(90, 200, 255, 230);
    dl->AddLine(center, tip, col, 2.0f);

    f32 dx = tip.x - center.x;
    f32 dy = tip.y - center.y;
    f32 len = sqrtf(dx * dx + dy * dy);
    if (len > 1.0f)
    {
        f32 ux = dx / len;
        f32 uy = dy / len;
        ImVec2 a(tip.x - ux * 8.0f + uy * 5.0f, tip.y - uy * 8.0f - ux * 5.0f);
        ImVec2 b(tip.x - ux * 8.0f - uy * 5.0f, tip.y - uy * 8.0f + ux * 5.0f);
        dl->AddTriangleFilled(tip, a, b, col);
    }

    dl->AddCircleFilled(tip, 5.0f, IM_COL32(255, 220, 0, 255), 0);

    if (ImGui::IsMouseClicked(0) && near(mouse, tip, 8.0f))
        begin_movement_edit(editor);

    if (editor->viewport.view_edit_drag == 6 && ImGui::IsMouseDown(0))
    {
        attack_set_movement(editor, editor->attack.attack_play_frame,
                            world_at(center, scale, mouse));
        mark_dirty(editor);
    }
}

void viewport_draw_attack_overlay(Editor* editor, ImDrawList* dl,
                                  const ImVec2& center, f32 scale)
{
    i32 shape_id = attack_hitbox_at(editor, editor->attack.attack_play_frame);
    i32 si = find_shape(editor, shape_id);

    /* Static shape (transparent blue fill + opaque blue border). */
    if (editor->attack.timeline_show_hitbox && si >= 0)
    {
        const ShapeEntry* sh = &editor->shapes.shapes[si];
        ImU32 fill = IM_COL32(60, 120, 255, 90);    /* transparent blue */
        ImU32 border = IM_COL32(60, 120, 255, 255); /* opaque blue */
        if (sh->type == 1)
        {
            f32 x0 = center.x + sh->pos.x * scale;
            f32 y1 = center.y - sh->pos.y * scale;
            f32 x1 = x0 + sh->size.x * scale;
            f32 y0 = y1 - sh->size.y * scale;
            dl->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), fill);
            dl->AddRect(ImVec2(x0, y0), ImVec2(x1, y1), border, 0.0f, 0, 2.0f);
        }
        else if (sh->type == 2)
        {
            if (sh->point_count >= 3)
            {
                ImVec2 pts[EDITOR_SHAPE_MAX_POINTS];
                for (i32 p = 0; p < sh->point_count; ++p)
                    pts[p] = screen_at(center, scale, sh->points[p]);
                dl->AddConvexPolyFilled(pts, sh->point_count, fill);
            }
            for (i32 p = 0; p < sh->point_count; ++p)
            {
                i32 q = (p + 1) % sh->point_count;
                dl->AddLine(screen_at(center, scale, sh->points[p]),
                            screen_at(center, scale, sh->points[q]), border, 2.0f);
            }
        }
        else
        {
            ImVec2 c = screen_at(center, scale, sh->center);
            dl->AddCircleFilled(c, sh->radius * scale, fill, 0);
            dl->AddCircle(c, sh->radius * scale, border, 0, 2.0f);
        }
    }

    if (editor->attack.timeline_show_hitbox && si >= 0)
        edit_shape(editor, dl, &si, center, scale);
    if (editor->attack.timeline_show_movement)
        edit_movement(editor, dl, center, scale);

    if (ImGui::IsMouseReleased(0))
        editor->viewport.view_edit_drag = 0;
}

} // namespace Editor
