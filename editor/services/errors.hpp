/**
 * @file errors.hpp
 * @brief Error popup handling for the editor.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Raise an error and open the error popup. */
void error_raise(Editor* editor, const char* message);

/** @brief Draw the error popup (modal); call every frame. */
void error_draw(Editor* editor);

/** @brief True when an error popup is currently active. */
bool error_active(const Editor* editor);

} // namespace Editor
