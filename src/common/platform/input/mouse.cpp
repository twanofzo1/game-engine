/**
 * @file mouse.cpp
 * @brief Mouse input state implementation.
 */
#include "mouse.hpp"

#include <string.h>

/** @brief Zero out all mouse state. */
void MouseInput::clear(MouseInput* mouse)
{
    memset(mouse, 0, sizeof(*mouse));
}

/** @brief Copy buttons into previous state and reset per-frame deltas. */
void MouseInput::roll(MouseInput* mouse)
{
    memcpy(mouse->mouse_buttons_prev, mouse->mouse_buttons,
           sizeof(mouse->mouse_buttons));

    mouse->mouse_dx = 0.0;
    mouse->mouse_dy = 0.0;
    mouse->scroll_x = 0.0;
    mouse->scroll_y = 0.0;
}

/** @brief Record a button press/release reported by GLFW. */
void MouseInput::on_button(MouseInput* mouse, int button, bool pressed)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return;
    mouse->mouse_buttons[button] = pressed;
}

/** @brief Record a cursor position update, accumulating the delta. */
void MouseInput::on_cursor(MouseInput* mouse, f64 x, f64 y)
{
    mouse->mouse_dx += x - mouse->mouse_x;
    mouse->mouse_dy += y - mouse->mouse_y;
    mouse->mouse_x = x;
    mouse->mouse_y = y;
}

/** @brief Record a scroll update, accumulating the delta. */
void MouseInput::on_scroll(MouseInput* mouse, f64 xoff, f64 yoff)
{
    mouse->scroll_x += xoff;
    mouse->scroll_y += yoff;
}

/** @brief True while `button` is held down. */
bool MouseInput::is_mouse_down(const MouseInput* mouse, int button)
{
    return mouse->mouse_buttons[button];
}

/** @brief True on the frame `button` transitions to held. */
bool MouseInput::is_mouse_pressed(const MouseInput* mouse, int button)
{
    return mouse->mouse_buttons[button] && !mouse->mouse_buttons_prev[button];
}

/** @brief True on the frame `button` transitions to released. */
bool MouseInput::is_mouse_released(const MouseInput* mouse, int button)
{
    return !mouse->mouse_buttons[button] && mouse->mouse_buttons_prev[button];
}
