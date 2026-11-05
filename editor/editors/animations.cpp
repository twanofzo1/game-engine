/**
 * @file animations.cpp
 * @brief Animation editor state helpers.
 */
#include "editors/animations.hpp"
#include "config/config_helpers.hpp"
#include "services/errors.hpp"
#include "services/notifications.hpp"
#include "services/undo.hpp"

#include "gameplay/animation/animation.hpp"
#include "serialization/animation_yaml.hpp"
#include "serialization/yaml.hpp"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace Engine;

namespace Editor {

void load_animations(Editor* editor, const char* config_path)
{
    editor->animation.animation_count = 0;

    Yaml::Node root;
    if (!Yaml::load(&root, config_path))
        return;

    Yaml::Node anims = root["animations"];
    if (!anims.IsSequence())
        return;

    for (usize i = 0; i < anims.size() && editor->animation.animation_count < EDITOR_MAX_ANIMATIONS; ++i)
    {
        Animation::Animation anim;
        Animation::init(&anim);

        Yaml::Node node = anims[i];
        if (!Yaml::load_animation(&node, &anim))
        {
            Animation::destroy(&anim);
            continue;
        }

        AnimationEntry* entry = &editor->animation.animations[editor->animation.animation_count];
        entry->yaml_index = (i32)i;
        snprintf(entry->name, EDITOR_ANIMATION_NAME_MAX, "%s", anim.name);
        entry->sprite_id = anim.sprite_id;

        entry->keyframe_count = 0;
        for (u32 k = 0; k < anim.frame_count && entry->keyframe_count < EDITOR_MAX_KEYFRAMES; ++k)
        {
            entry->keyframes[entry->keyframe_count].time = anim.frames[k].time;
            entry->keyframes[entry->keyframe_count].frame = anim.frames[k].frame;
            entry->keyframe_count++;
        }

        Animation::destroy(&anim);
        editor->animation.animation_count++;
    }
}

void save_animation(Editor* editor, const char* config_path)
{
    if (editor->animation.anim_name[0] == '\0')
    {
        error_raise(editor, "name required");
        return;
    }
    if (editor->animation.anim_keyframe_count == 0)
    {
        error_raise(editor, "no keyframes");
        return;
    }

    Animation::Animation anim;
    Animation::init(&anim);
    snprintf(anim.name, sizeof(anim.name), "%s", editor->animation.anim_name);
    anim.sprite_id = editor->animation.anim_sprite_id;
    for (i32 k = 0; k < editor->animation.anim_keyframe_count; ++k)
        Animation::add_frame(&anim, editor->animation.anim_keyframes[k].time,
                             editor->animation.anim_keyframes[k].frame);

    Yaml::Node root = config_open(config_path);
    bool replaced = config_upsert_by_name(&root, "animations",
                                          Yaml::make_animation(&anim),
                                          editor->animation.anim_name,
                                          editor->animation.anim_sprite_id);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, replaced ? "updated animation" : "saved animation");
        editor->guard.dirty = false;
        load_animations(editor, config_path);
    }
    else
    {
        error_raise(editor, "save failed");
    }

    Animation::destroy(&anim);
}

void delete_animation_at(Editor* editor, const char* config_path, i32 yaml_index)
{
    Yaml::Node root = config_open(config_path);
    Yaml::Node anims = root["animations"];
    if (!anims.IsSequence() || yaml_index < 0 || (usize)yaml_index >= anims.size())
    {
        error_raise(editor, "animation not found");
        return;
    }

    root["animations"] = config_remove_index(anims, yaml_index);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, "deleted animation");
        load_animations(editor, config_path);
        editor->animation.selected_animation = -1;
    }
    else
    {
        error_raise(editor, "save failed");
    }
}

void animation_add_keyframe(Editor* editor, f32 time, i32 frame)
{
    if (editor->animation.anim_keyframe_count >= EDITOR_MAX_KEYFRAMES)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    /* Insert sorted by time so the timeline and playback stay ordered. */
    i32 i = editor->animation.anim_keyframe_count;
    while (i > 0 && editor->animation.anim_keyframes[i - 1].time > time)
    {
        editor->animation.anim_keyframes[i] = editor->animation.anim_keyframes[i - 1];
        --i;
    }
    editor->animation.anim_keyframes[i].time = time;
    editor->animation.anim_keyframes[i].frame = frame;
    editor->animation.anim_keyframe_count++;
}

void animation_remove_keyframe(Editor* editor, i32 index)
{
    if (index < 0 || index >= editor->animation.anim_keyframe_count)
        return;

    undo_push(editor);
    editor->guard.dirty = true;

    for (i32 k = index; k + 1 < editor->animation.anim_keyframe_count; ++k)
        editor->animation.anim_keyframes[k] = editor->animation.anim_keyframes[k + 1];
    editor->animation.anim_keyframe_count--;
}

void animation_clear(Editor* editor)
{
    undo_push(editor);
    editor->guard.dirty = true;

    editor->animation.selected_animation = -1;
    editor->animation.anim_name[0] = '\0';
    editor->animation.anim_frame = 0;
    editor->animation.anim_time = 0.0f;
    editor->animation.anim_keyframe_count = 0;
    editor->animation.anim_playing = false;
    editor->animation.anim_play_time = 0.0f;
    editor->state = STATE_ANIMATION;
}

/** @brief Largest keyframe time in the edited animation (0 when empty). */
static f32 max_keyframe_time(const Editor* editor)
{
    f32 d = 0.0f;
    for (i32 k = 0; k < editor->animation.anim_keyframe_count; ++k)
        if (editor->animation.anim_keyframes[k].time > d)
            d = editor->animation.anim_keyframes[k].time;
    return d;
}

f32 animation_duration(const Editor* editor)
{
    f32 d = max_keyframe_time(editor);
    return d > 0.0f ? d : 1.0f;
}

i32 animation_frame_at(const Editor* editor, f32 time)
{
    if (editor->animation.anim_keyframe_count == 0)
        return 0;

    f32 d = max_keyframe_time(editor);
    if (d > 0.0f && time >= d)
        time = fmodf(time, d);

    i32 frame = editor->animation.anim_keyframes[0].frame;
    for (i32 k = 0; k < editor->animation.anim_keyframe_count; ++k)
    {
        if (time >= editor->animation.anim_keyframes[k].time)
            frame = editor->animation.anim_keyframes[k].frame;
        else
            break;
    }
    return frame;
}

void animation_tick(Editor* editor, f32 dt)
{
    if (!editor->animation.anim_playing)
        return;

    editor->animation.anim_play_time += dt * editor->animation.anim_speed;
    f32 d = max_keyframe_time(editor);
    if (d > 0.0f && editor->animation.anim_play_time >= d)
        editor->animation.anim_play_time = fmodf(editor->animation.anim_play_time, d);

    editor->preview.single_frame = true;
    editor->preview.frame = animation_frame_at(editor, editor->animation.anim_play_time);
}

void open_animation(Editor* editor, i32 anim_index)
{
    if (anim_index < 0 || anim_index >= editor->animation.animation_count)
        return;

    AnimationEntry* anim = &editor->animation.animations[anim_index];

    for (i32 s = 0; s < editor->sprites.sprite_count; ++s)
    {
        if (editor->sprites.sprites[s].yaml_index == anim->sprite_id)
        {
            editor->sprites.selected_sprite = s;
            open_sprite(editor, &editor->sprites.sprites[s]);
            break;
        }
    }

    editor->animation.selected_animation = anim_index;
    editor->state = STATE_ANIMATION;
    editor->animation.anim_sprite_id = anim->sprite_id;
    snprintf(editor->animation.anim_name, EDITOR_ANIMATION_NAME_MAX, "%s", anim->name);
    editor->animation.anim_keyframe_count = anim->keyframe_count;
    for (i32 k = 0; k < anim->keyframe_count; ++k)
        editor->animation.anim_keyframes[k] = anim->keyframes[k];

    editor->animation.anim_playing = false;
    editor->animation.anim_play_time = 0.0f;
    editor->preview.single_frame = true;
    editor->preview.frame = animation_frame_at(editor, 0.0f);
}

void rename_animation(Editor* editor, const char* config_path, i32 yaml_index,
                      const char* new_name)
{
    if (new_name[0] == '\0')
    {
        error_raise(editor, "name required");
        return;
    }

    Yaml::Node root = config_open(config_path);
    Yaml::Node anims = root["animations"];
    if (!anims.IsSequence() || yaml_index < 0 || (usize)yaml_index >= anims.size())
    {
        error_raise(editor, "animation not found");
        return;
    }

    config_rename_at(&root, "animations", yaml_index, new_name);

    if (Yaml::save(&root, config_path))
    {
        notify(editor, "renamed animation");
        load_animations(editor, config_path);
    }
    else
    {
        error_raise(editor, "save failed");
    }
}

} // namespace Editor
