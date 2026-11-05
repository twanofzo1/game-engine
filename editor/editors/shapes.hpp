/**
 * @file shapes.hpp
 * @brief Collision-shape (hitbox) editor helpers: load/save/delete and editing.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Reload the collision-shape list from the config file. */
void load_shapes(Editor* editor, const char* config_path);

/** @brief Write all shapes back to the config file. */
void save_shapes(Editor* editor, const char* config_path);

/** @brief Remove the shape at `yaml_index` from the config file. */
void delete_shape_at(Editor* editor, const char* config_path, i32 yaml_index);

/** @brief Append a fresh default circle and select it. */
void shape_new(Editor* editor);

/** @brief Duplicate the shape at `index`; return its yaml index, or -1. */
i32 shape_duplicate(Editor* editor, i32 index);

/** @brief Select the shape at `index` (into `shapes`). */
void shape_select(Editor* editor, i32 index);

} // namespace Editor
