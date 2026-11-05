/**
 * @file guard.hpp
 * @brief Unsaved-changes guard: prompts to save/discard before switching.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Mark the current editing context as modified. */
void mark_dirty(Editor* editor);

/** @brief Request to open an image (deferred if there are unsaved changes). */
void request_open_image(Editor* editor, const char* path);

/** @brief Request to open a sprite (deferred if there are unsaved changes). */
void request_open_sprite(Editor* editor, i32 sprite_index);

/** @brief Request to open an animation (deferred if unsaved changes). */
void request_open_animation(Editor* editor, i32 anim_index);

/** @brief Request to start a new animation (deferred if unsaved changes). */
void request_new_animation(Editor* editor);

/** @brief Request to open an attack (deferred if unsaved changes). */
void request_open_attack(Editor* editor, i32 attack_index);

/** @brief Request to start a new attack (deferred if unsaved changes). */
void request_new_attack(Editor* editor);

/** @brief Draw the save/discard popup; call every frame. */
void guard_draw(Editor* editor);

} // namespace Editor
