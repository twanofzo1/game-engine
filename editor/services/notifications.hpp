/**
 * @file notifications.hpp
 * @brief Non-blocking notification queue for the editor.
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** @brief Push a notification message. */
void notify(Editor* editor, const char* message);

/** @brief Draw the notification toasts; call every frame. */
void notifications_draw(Editor* editor);

} // namespace Editor
