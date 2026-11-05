/**
 * @file dnd.hpp
 * @brief Dear ImGui drag-and-drop helpers for the editor's reference slots.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** Payload type carrying an image path string. */
#define DND_IMAGE_PATH "EDITOR_IMAGE_PATH"
/** Payload type carrying a sprite yaml index. */
#define DND_SPRITE     "EDITOR_SPRITE"
/** Payload type carrying an animation yaml index. */
#define DND_ANIMATION  "EDITOR_ANIMATION"
/** Payload type carrying a shape yaml index. */
#define DND_SHAPE      "EDITOR_SHAPE"

/** @brief Draw a labelled slot accepting an i32 payload; true when dropped. */
bool dnd_slot_i32(const char* label, const char* payload, const char* value_text,
                  i32* value);

/** @brief Draw a labelled slot accepting a string payload; true when dropped. */
bool dnd_slot_string(const char* label, const char* payload, const char* value_text,
                     char* out, i32 size);

/** @brief Start a drag source carrying an i32 value (call after a widget). */
void dnd_source_i32(const char* payload, i32 value, const char* preview);

/** @brief Start a drag source carrying a string value. */
void dnd_source_string(const char* payload, const char* value);

} // namespace Editor
