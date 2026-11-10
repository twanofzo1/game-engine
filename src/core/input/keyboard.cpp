/**
 * @file keyboard.cpp
 * @brief Keyboard input state implementation.
 */
#include "keyboard.hpp"

#include <string.h>

/** @brief Zero out all key state. */
void KeyboardInput::clear(KeyboardInput* kb)
{
    memset(kb, 0, sizeof(*kb));
}

/** @brief Copy the current frame into the previous frame. */
void KeyboardInput::roll(KeyboardInput* kb)
{
    memcpy(kb->keys_prev, kb->keys, sizeof(kb->keys));
}

/** @brief Record a key press/release reported by GLFW. */
void KeyboardInput::on_key(KeyboardInput* kb, int key, bool pressed)
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return;
    kb->keys[key] = pressed;
}

/** @brief True while `key` is held down. */
bool KeyboardInput::is_key_down(const KeyboardInput* kb, int key)
{
    return kb->keys[key];
}

/** @brief True on the frame `key` transitions to held. */
bool KeyboardInput::is_key_pressed(const KeyboardInput* kb, int key)
{
    return kb->keys[key] && !kb->keys_prev[key];
}

/** @brief True on the frame `key` transitions to released. */
bool KeyboardInput::is_key_released(const KeyboardInput* kb, int key)
{
    return !kb->keys[key] && kb->keys_prev[key];
}
