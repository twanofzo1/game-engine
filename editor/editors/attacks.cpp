/**
 * @file attacks.cpp
 * @brief Attack editor helpers.
 */
#include "editors/attacks.hpp"
#include "config/config_helpers.hpp"
#include "services/errors.hpp"
#include "services/notifications.hpp"
#include "editors/shapes.hpp"
#include "services/undo.hpp"

#include "gameplay/attack/attack.hpp"
#include "serialization/attack_yaml.hpp"
#include "serialization/yaml.hpp"

#include <stdio.h>
#include <string.h>
#include <string>

using namespace Engine;

namespace Editor {

void load_attacks(Editor* editor, const char* config_path)
{
    editor->attack.attack_count = 0;

    Yaml::Node root;
    if (!Yaml::load(&root, config_path))
        return;

    Yaml::Node attacks = root["attacks"];
    if (!attacks.IsSequence())
        return;

    for (usize i = 0; i < attacks.size() && editor->attack.attack_count < EDITOR_MAX_ATTACKS; ++i)
    {
        Attack::Attack atk;
        Attack::init(&atk);

        Yaml::Node node = attacks[i];
        if (!Yaml::load_attack(&node, &atk))
        {
            Attack::destroy(&atk);
            continue;
        }

        AttackEntry* entry = &editor->attack.attacks[editor->attack.attack_count];
        entry->yaml_index = (i32)i;
        snprintf(entry->name, EDITOR_SPRITE_NAME_MAX, "%s", atk.name);
        entry->animation_id = atk.animation_id;
        entry->windup = atk.windup;
        entry->active = atk.active;
        entry->recovery = atk.recovery;

        entry->event_count = 0;
        for (u32 e = 0; e < atk.event_count && entry->event_count < EDITOR_MAX_ATTACK_EVENTS; ++e)
        {
            entry->events[entry->event_count].frame = atk.events[e].frame;
            snprintf(entry->events[entry->event_count].name, EDITOR_EVENT_NAME_MAX, "%s",
                     atk.events[e].name);
            entry->event_count++;
        }

        entry->hitbox_count = 0;
        for (u32 h = 0; h < atk.hitbox_count && entry->hitbox_count < EDITOR_MAX_ATTACK_HITBOXES; ++h)
        {
            entry->hitboxes[entry->hitbox_count].frame = atk.hitboxes[h].frame;
            entry->hitboxes[entry->hitbox_count].shape_id = atk.hitboxes[h].shape_id;
            entry->hitbox_count++;
        }

        entry->movement_count = 0;
        for (u32 m = 0; m < atk.movement_count && entry->movement_count < EDITOR_MAX_ATTACK_MOVEMENTS; ++m)
        {
            entry->movements[entry->movement_count].frame = atk.movements[m].frame;
            entry->movements[entry->movement_count].vector = atk.movements[m].vector;
            entry->movement_count++;
        }

        Attack::destroy(&atk);
        editor->attack.attack_count++;
    }
}

void save_attack(Editor* editor, const char* config_path)
{
    if (editor->attack.attack_name[0] == '\0')
    {
        error_raise(editor, "name required");
        return;
    }

    Attack::Attack atk;
    Attack::init(&atk);
    strncpy(atk.name, editor->attack.attack_name, sizeof(atk.name) - 1);
    atk.name[sizeof(atk.name) - 1] = '\0';
    atk.animation_id = editor->attack.attack_animation_id;
    atk.windup = editor->attack.attack_windup;
    atk.active = editor->attack.attack_active;
    atk.recovery = editor->attack.attack_recovery;

    /* Persist shape edits (made in the viewport) alongside the attack. */
    save_shapes(editor, config_path);

    for (i32 e = 0; e < editor->attack.attack_event_count; ++e)
        Attack::add_event(&atk, editor->attack.attack_events[e].frame,
                          editor->attack.attack_events[e].name);
    for (i32 h = 0; h < editor->attack.attack_hitbox_count; ++h)
        Attack::add_hitbox(&atk, editor->attack.attack_hitboxes[h].frame,
                           editor->attack.attack_hitboxes[h].shape_id);
    for (i32 m = 0; m < editor->attack.attack_movement_count; ++m)
        Attack::add_movement(&atk, editor->attack.attack_movements[m].frame,
                             editor->attack.attack_movements[m].vector);

    Yaml::Node root = config_open(config_path);
    bool replaced = config_upsert_by_name(&root, "attacks",
                                          Yaml::make_attack(&atk),
                                          editor->attack.attack_name, -1);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, replaced ? "updated attack" : "saved attack");
        editor->guard.dirty = false;
        load_attacks(editor, config_path);
    }
    else
    {
        error_raise(editor, "save failed");
    }

    Attack::destroy(&atk);
}

void delete_attack_at(Editor* editor, const char* config_path, i32 yaml_index)
{
    Yaml::Node root = config_open(config_path);
    Yaml::Node attacks = root["attacks"];
    if (!attacks.IsSequence() || yaml_index < 0 || (usize)yaml_index >= attacks.size())
    {
        error_raise(editor, "attack not found");
        return;
    }

    root["attacks"] = config_remove_index(attacks, yaml_index);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, "deleted attack");
        load_attacks(editor, config_path);
        editor->attack.selected_attack = -1;
    }
    else
    {
        error_raise(editor, "save failed");
    }
}

void rename_attack(Editor* editor, const char* config_path, i32 yaml_index,
                   const char* new_name)
{
    if (new_name[0] == '\0')
    {
        error_raise(editor, "name required");
        return;
    }

    Yaml::Node root = config_open(config_path);
    Yaml::Node attacks = root["attacks"];
    if (!attacks.IsSequence() || yaml_index < 0 || (usize)yaml_index >= attacks.size())
    {
        error_raise(editor, "attack not found");
        return;
    }

    config_rename_at(&root, "attacks", yaml_index, new_name);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, "renamed attack");
        load_attacks(editor, config_path);
    }
    else
    {
        error_raise(editor, "save failed");
    }
}

void attack_add_event(Editor* editor, i32 frame, const char* name)
{
    if (editor->attack.attack_event_count >= EDITOR_MAX_ATTACK_EVENTS)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    i32 i = editor->attack.attack_event_count;
    while (i > 0 && editor->attack.attack_events[i - 1].frame > frame)
    {
        editor->attack.attack_events[i] = editor->attack.attack_events[i - 1];
        --i;
    }
    editor->attack.attack_events[i].frame = frame;
    snprintf(editor->attack.attack_events[i].name, EDITOR_EVENT_NAME_MAX, "%s",
             name ? name : "");
    editor->attack.attack_event_count++;
}

void attack_remove_event(Editor* editor, i32 index)
{
    if (index < 0 || index >= editor->attack.attack_event_count)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    for (i32 e = index; e + 1 < editor->attack.attack_event_count; ++e)
        editor->attack.attack_events[e] = editor->attack.attack_events[e + 1];
    editor->attack.attack_event_count--;
}

void attack_add_hitbox(Editor* editor, i32 frame, i32 shape_id)
{
    if (editor->attack.attack_hitbox_count >= EDITOR_MAX_ATTACK_HITBOXES)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    i32 i = editor->attack.attack_hitbox_count;
    while (i > 0 && editor->attack.attack_hitboxes[i - 1].frame > frame)
    {
        editor->attack.attack_hitboxes[i] = editor->attack.attack_hitboxes[i - 1];
        --i;
    }
    editor->attack.attack_hitboxes[i].frame = frame;
    editor->attack.attack_hitboxes[i].shape_id = shape_id;
    editor->attack.attack_hitbox_count++;
}

void attack_remove_hitbox(Editor* editor, i32 index)
{
    if (index < 0 || index >= editor->attack.attack_hitbox_count)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    for (i32 h = index; h + 1 < editor->attack.attack_hitbox_count; ++h)
        editor->attack.attack_hitboxes[h] = editor->attack.attack_hitboxes[h + 1];
    editor->attack.attack_hitbox_count--;
}

void attack_set_hitbox(Editor* editor, i32 frame, i32 shape_id)
{
    for (i32 h = 0; h < editor->attack.attack_hitbox_count; ++h)
    {
        if (editor->attack.attack_hitboxes[h].frame == frame)
        {
            editor->attack.attack_hitboxes[h].shape_id = shape_id;
            return;
        }
    }

    if (editor->attack.attack_hitbox_count >= EDITOR_MAX_ATTACK_HITBOXES)
        return;

    i32 i = editor->attack.attack_hitbox_count;
    while (i > 0 && editor->attack.attack_hitboxes[i - 1].frame > frame)
    {
        editor->attack.attack_hitboxes[i] = editor->attack.attack_hitboxes[i - 1];
        --i;
    }
    editor->attack.attack_hitboxes[i].frame = frame;
    editor->attack.attack_hitboxes[i].shape_id = shape_id;
    editor->attack.attack_hitbox_count++;
}

void attack_add_movement(Editor* editor, i32 frame, Vec2::Vec2 vector)
{
    if (editor->attack.attack_movement_count >= EDITOR_MAX_ATTACK_MOVEMENTS)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    i32 i = editor->attack.attack_movement_count;
    while (i > 0 && editor->attack.attack_movements[i - 1].frame > frame)
    {
        editor->attack.attack_movements[i] = editor->attack.attack_movements[i - 1];
        --i;
    }
    editor->attack.attack_movements[i].frame = frame;
    editor->attack.attack_movements[i].vector = vector;
    editor->attack.attack_movement_count++;
}

void attack_set_movement(Editor* editor, i32 frame, Vec2::Vec2 vector)
{
    for (i32 m = 0; m < editor->attack.attack_movement_count; ++m)
    {
        if (editor->attack.attack_movements[m].frame == frame)
        {
            editor->attack.attack_movements[m].vector = vector;
            return;
        }
    }

    if (editor->attack.attack_movement_count >= EDITOR_MAX_ATTACK_MOVEMENTS)
        return;

    i32 i = editor->attack.attack_movement_count;
    while (i > 0 && editor->attack.attack_movements[i - 1].frame > frame)
    {
        editor->attack.attack_movements[i] = editor->attack.attack_movements[i - 1];
        --i;
    }
    editor->attack.attack_movements[i].frame = frame;
    editor->attack.attack_movements[i].vector = vector;
    editor->attack.attack_movement_count++;
}

void attack_remove_movement(Editor* editor, i32 index)
{
    if (index < 0 || index >= editor->attack.attack_movement_count)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    for (i32 m = index; m + 1 < editor->attack.attack_movement_count; ++m)
        editor->attack.attack_movements[m] = editor->attack.attack_movements[m + 1];
    editor->attack.attack_movement_count--;
}

void attack_clear(Editor* editor)
{
    undo_push(editor);
    editor->guard.dirty = true;

    editor->attack.selected_attack = -1;
    editor->attack.attack_name[0] = '\0';
    editor->attack.attack_animation_id = -1;
    editor->attack.attack_windup = 5;
    editor->attack.attack_active = 3;
    editor->attack.attack_recovery = 5;
    editor->attack.attack_event_count = 0;
    editor->attack.attack_hitbox_count = 0;
    editor->attack.attack_movement_count = 0;
    editor->attack.attack_playing = false;
    editor->attack.attack_play_frame = 0;
    editor->attack.attack_play_accum = 0.0f;
    editor->state = STATE_ATTACK;
}

i32 attack_total_duration(const Editor* editor)
{
    i32 d = editor->attack.attack_windup + editor->attack.attack_active + editor->attack.attack_recovery;
    return d > 0 ? d : 1;
}

/** @brief The animation entry whose yaml index equals `animation_id`. */
static const AnimationEntry* find_animation(const Editor* editor, i32 animation_id)
{
    for (i32 a = 0; a < editor->animation.animation_count; ++a)
        if (editor->animation.animations[a].yaml_index == animation_id)
            return &editor->animation.animations[a];
    return NULL;
}

i32 attack_frame_at(const Editor* editor, i32 frame)
{
    const AnimationEntry* anim = find_animation(editor, editor->attack.attack_animation_id);
    if (anim && anim->keyframe_count > 0)
    {
        i32 k = frame;
        if (k < 0) k = 0;
        if (k >= anim->keyframe_count) k = anim->keyframe_count - 1;
        return anim->keyframes[k].frame;
    }
    return frame;
}

i32 attack_hitbox_at(const Editor* editor, i32 frame)
{
    i32 shape = -1;
    for (i32 h = 0; h < editor->attack.attack_hitbox_count; ++h)
    {
        if (frame >= editor->attack.attack_hitboxes[h].frame)
            shape = editor->attack.attack_hitboxes[h].shape_id;
        else
            break;
    }
    return shape;
}

Vec2::Vec2 attack_movement_at(const Editor* editor, i32 frame)
{
    Vec2::Vec2 v = Vec2::make(0.0f, 0.0f);
    for (i32 m = 0; m < editor->attack.attack_movement_count; ++m)
    {
        if (frame >= editor->attack.attack_movements[m].frame)
            v = editor->attack.attack_movements[m].vector;
        else
            break;
    }
    return v;
}

void attack_tick(Editor* editor, f32 dt)
{
    if (!editor->attack.attack_playing)
        return;

    /* Advance by frames at ~12 fps, scaled by speed. */
    editor->attack.attack_play_accum += dt * editor->attack.attack_speed * 12.0f;
    while (editor->attack.attack_play_accum >= 1.0f)
    {
        editor->attack.attack_play_accum -= 1.0f;
        editor->attack.attack_play_frame++;
    }

    i32 total = attack_total_duration(editor);
    if (total > 0 && editor->attack.attack_play_frame >= total)
        editor->attack.attack_play_frame = editor->attack.attack_play_frame % total;

    editor->preview.single_frame = true;
    editor->preview.frame = attack_frame_at(editor, editor->attack.attack_play_frame);
}

void open_attack(Editor* editor, i32 attack_index)
{
    if (attack_index < 0 || attack_index >= editor->attack.attack_count)
        return;

    const AttackEntry* atk = &editor->attack.attacks[attack_index];

    /* Load the sprite driven by the attack's animation. */
    const AnimationEntry* anim = find_animation(editor, atk->animation_id);
    if (anim)
    {
        for (i32 s = 0; s < editor->sprites.sprite_count; ++s)
        {
            if (editor->sprites.sprites[s].yaml_index == anim->sprite_id)
            {
                editor->sprites.selected_sprite = s;
                open_sprite(editor, &editor->sprites.sprites[s]);
                break;
            }
        }
    }

    editor->attack.selected_attack = attack_index;
    editor->state = STATE_ATTACK;
    editor->attack.attack_animation_id = atk->animation_id;
    editor->attack.attack_windup = atk->windup;
    editor->attack.attack_active = atk->active;
    editor->attack.attack_recovery = atk->recovery;
    snprintf(editor->attack.attack_name, EDITOR_SPRITE_NAME_MAX, "%s", atk->name);

    editor->attack.attack_event_count = atk->event_count;
    for (i32 e = 0; e < atk->event_count; ++e)
        editor->attack.attack_events[e] = atk->events[e];

    editor->attack.attack_hitbox_count = atk->hitbox_count;
    for (i32 h = 0; h < atk->hitbox_count; ++h)
        editor->attack.attack_hitboxes[h] = atk->hitboxes[h];

    editor->attack.attack_movement_count = atk->movement_count;
    for (i32 m = 0; m < atk->movement_count; ++m)
        editor->attack.attack_movements[m] = atk->movements[m];

    editor->attack.attack_playing = false;
    editor->attack.attack_play_frame = 0;
    editor->attack.attack_play_accum = 0.0f;
    editor->preview.single_frame = true;
    editor->preview.frame = attack_frame_at(editor, 0);
}

} // namespace Editor
