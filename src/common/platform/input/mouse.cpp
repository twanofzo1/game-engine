/**
 * @file mouse.cpp
 * @brief Mouse input state implementation.
 */
#include "mouse.hpp"

#include <string.h>

/// @brief Zeroes out all mouse state.
void MouseInput::clear()
{
    memset(this, 0, sizeof(*this));
}

/// @brief Copies buttons into previous state and resets per-frame deltas.
void MouseInput::roll()
{
    memcpy(this->mouse_buttons_prev, this->mouse_buttons,
           sizeof(this->mouse_buttons));

    this->mouse_dx = 0.0;
    this->mouse_dy = 0.0;
    this->scroll_x = 0.0;
    this->scroll_y = 0.0;
}

/// @brief Records a button press or release reported by GLFW.
/// @param button The button code (ignored when out of range).
/// @param pressed True on press, false on release.
void MouseInput::on_button(const int button, const bool pressed)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return;
    this->mouse_buttons[button] = pressed;
}

/// @brief Records a cursor position update, accumulating the delta.
/// @param x The new cursor X coordinate.
/// @param y The new cursor Y coordinate.
void MouseInput::on_cursor(const f64 x, const f64 y)
{
    this->mouse_dx += x - this->mouse_x;
    this->mouse_dy += y - this->mouse_y;
    this->mouse_x = x;
    this->mouse_y = y;
}

/// @brief Records a scroll update, accumulating the delta.
/// @param xoff The horizontal scroll delta.
/// @param yoff The vertical scroll delta.
void MouseInput::on_scroll(const f64 xoff, const f64 yoff)
{
    this->scroll_x += xoff;
    this->scroll_y += yoff;
}

/// @brief Returns true while `button` is held down.
/// @param button The button code.
/// @return True when the button is currently held.
bool MouseInput::is_mouse_down(const int button) const
{
    return this->mouse_buttons[button];
}

/// @brief Returns true on the frame `button` transitions to held.
/// @param button The button code.
/// @return True when the button was pressed this frame.
bool MouseInput::is_mouse_pressed(const int button) const
{
    return this->mouse_buttons[button] && !this->mouse_buttons_prev[button];
}

/// @brief Returns true on the frame `button` transitions to released.
/// @param button The button code.
/// @return True when the button was released this frame.
bool MouseInput::is_mouse_released(const int button) const
{
    return !this->mouse_buttons[button] && this->mouse_buttons_prev[button];
}
