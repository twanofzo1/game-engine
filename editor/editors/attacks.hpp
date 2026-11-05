/**
 * @file attacks.hpp
 * @brief Attack editor helpers: load/save/delete, keyframe edits and playback.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Reload the attack list from the config file. */
void load_attacks(Editor* editor, const char* config_path);

/** @brief Save the attack being edited (overwrite by name, else append). */
void save_attack(Editor* editor, const char* config_path);

/** @brief Remove the attack at `yaml_index` from the config file. */
void delete_attack_at(Editor* editor, const char* config_path, i32 yaml_index);

/** @brief Rename the attack at `yaml_index`. */
void rename_attack(Editor* editor, const char* config_path, i32 yaml_index,
                   const char* new_name);

/** @brief Append an event keyframe (sorted by frame). */
void attack_add_event(Editor* editor, i32 frame, const char* name);

/** @brief Remove an event keyframe. */
void attack_remove_event(Editor* editor, i32 index);

/** @brief Append a hitbox keyframe (sorted by frame). */
void attack_add_hitbox(Editor* editor, i32 frame, i32 shape_id);

/** @brief Replace the hitbox keyframe at `frame`, or append a new one. */
void attack_set_hitbox(Editor* editor, i32 frame, i32 shape_id);

/** @brief Remove a hitbox keyframe. */
void attack_remove_hitbox(Editor* editor, i32 index);

/** @brief Append a movement keyframe (sorted by frame). */
void attack_add_movement(Editor* editor, i32 frame, Engine::Vec2::Vec2 vector);

/** @brief Replace the movement keyframe at `frame`, or append a new one. */
void attack_set_movement(Editor* editor, i32 frame, Engine::Vec2::Vec2 vector);

/** @brief Remove a movement keyframe. */
void attack_remove_movement(Editor* editor, i32 index);

/** @brief Reset the editing buffer for a fresh attack. */
void attack_clear(Editor* editor);

/** @brief Total attack length in frames (windup + active + recovery, >= 1). */
i32 attack_total_duration(const Editor* editor);

/** @brief Sprite frame shown at attack frame `frame`. */
i32 attack_frame_at(const Editor* editor, i32 frame);

/** @brief Active hitbox shape id at `frame`, or -1 before the first keyframe. */
i32 attack_hitbox_at(const Editor* editor, i32 frame);

/** @brief Movement vector at `frame`. */
Engine::Vec2::Vec2 attack_movement_at(const Editor* editor, i32 frame);

/** @brief Advance playback by `dt` and update the preview frame. */
void attack_tick(Editor* editor, f32 dt);

/** @brief Open a saved attack into the editor. */
void open_attack(Editor* editor, i32 attack_index);

} // namespace Editor
