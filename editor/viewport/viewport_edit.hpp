/**
 * @file viewport_edit.hpp
 * @brief Direct manipulation of hitboxes and movement in the viewport.
 */
#pragma once

#include "core/editor.hpp"
#include "imgui.h"

namespace Editor {

/**
 * @brief Draw (and edit) the active hitbox + movement vector over the sprite.
 * @param center Screen-space center of the sprite preview.
 * @param scale  World units per pixel (matches the preview zoom).
 */
void viewport_draw_attack_overlay(Editor* editor, ImDrawList* dl,
                                  const ImVec2& center, f32 scale);

} // namespace Editor
