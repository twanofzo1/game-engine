/**
 * @file animations.hpp
 * @brief Animation editor state helpers (load/save/delete and keyframe edits).
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Reload the animation list from the config file. */
void load_animations(Editor* editor, const char* config_path);

/** @brief Save the animation being edited (overwrite by name, else append). */
void save_animation(Editor* editor, const char* config_path);

/** @brief Remove the animation at `yaml_index` from the config file. */
void delete_animation_at(Editor* editor, const char* config_path, i32 yaml_index);

/** @brief Rename the animation at `yaml_index`. */
void rename_animation(Editor* editor, const char* config_path, i32 yaml_index,
                      const char* new_name);

/** @brief Append a keyframe to the animation being edited. */
void animation_add_keyframe(Editor* editor, f32 time, i32 frame);

/** @brief Remove a keyframe from the animation being edited. */
void animation_remove_keyframe(Editor* editor, i32 index);

/** @brief Reset the editing buffer for a fresh animation. */
void animation_clear(Editor* editor);

/** @brief Duration of the animation being edited (max keyframe time). */
f32 animation_duration(const Editor* editor);

/** @brief Frame shown at `time` (loops around the duration). */
i32 animation_frame_at(const Editor* editor, f32 time);

/** @brief Advance playback by `dt` and update the preview frame. */
void animation_tick(Editor* editor, f32 dt);

/** @brief Open a saved animation (sprite + keyframes) into the editor. */
void open_animation(Editor* editor, i32 anim_index);

} // namespace Editor
