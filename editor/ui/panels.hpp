/**
 * @file panels.hpp
 * @brief Declares the editor's Dear ImGui panels.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Draw the "Assets" window: image file tree. */
void draw_assets(Editor* editor);

/** @brief Draw the "Sprites" window: sprite folders with their animations. */
void draw_sprites(Editor* editor);

/** @brief Draw the "Viewport" window: sprite sheet preview. */
void draw_viewport(Editor* editor);

/** @brief Draw the "Options" window: sprite settings and save button. */
void draw_options(Editor* editor);

/** @brief Draw the "Animations" window: animation timeline editor. */
void draw_animations(Editor* editor);

/** @brief Draw the "Attacks" window: attack timeline editor. */
void draw_attacks(Editor* editor);

/** @brief Draw the attack-editing controls (inside the Options window). */
void draw_attack_options(Editor* editor);

} // namespace Editor
