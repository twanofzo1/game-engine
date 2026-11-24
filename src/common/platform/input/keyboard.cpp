/**
 * @file keyboard.cpp
 * @brief Keyboard input state implementation.
 */
#include "keyboard.hpp"

#include <string.h>

/// @brief Zeroes out all key state.
void KeyboardInput::clear()
{
    memset(this, 0, sizeof(*this));
}

/// @brief Copies the current frame's keys into the previous frame.
void KeyboardInput::roll()
{
    memcpy(this->keys_prev, this->keys, sizeof(this->keys));
}

/// @brief Records a key press or release reported by GLFW.
/// @param key The key code (ignored when out of range).
/// @param pressed True on press, false on release.
void KeyboardInput::on_key(const int key, const bool pressed)
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return;
    this->keys[key] = pressed;
}

/// @brief Returns true while `key` is held down.
/// @param key The key code.
/// @return True when the key is currently held.
bool KeyboardInput::is_key_down(const int key) const
{
    return this->keys[key];
}

/// @brief Returns true on the frame `key` transitions to held.
/// @param key The key code.
/// @return True when the key was pressed this frame.
bool KeyboardInput::is_key_pressed(const int key) const
{
    return this->keys[key] && !this->keys_prev[key];
}

/// @brief Returns true on the frame `key` transitions to released.
/// @param key The key code.
/// @return True when the key was released this frame.
bool KeyboardInput::is_key_released(const int key) const
{
    return !this->keys[key] && this->keys_prev[key];
}
