#include "input.h"
#include <string.h>

static bool key_down_current[KEY_COUNT];
static bool key_down_previous[KEY_COUNT];

static bool mouse_down_current[MOUSE_BUTTON_COUNT];
static bool mouse_down_previous[MOUSE_BUTTON_COUNT];

static f32 mouse_x_current;
static f32 mouse_y_current;
static f32 mouse_x_previous;
static f32 mouse_y_previous;

void InputInit()
{
    memset(key_down_current, 0, sizeof(key_down_current));
    memset(key_down_previous, 0, sizeof(key_down_previous));

    memset(mouse_down_current, 0, sizeof(mouse_down_current));
    memset(mouse_down_previous, 0, sizeof(mouse_down_previous));

    mouse_x_current = 0.0f;
    mouse_y_current = 0.0f;
    mouse_x_previous = 0.0f;
    mouse_y_previous = 0.0f;
}

void InputUpdate(InputState* state)
{
    for (int i = 0; i < KEY_COUNT; ++i)
    {
        state->key_down[i] = key_down_current[i];
        state->key_pressed[i] = key_down_current[i] && !key_down_previous[i];
        state->key_released[i] = !key_down_current[i] && key_down_previous[i];

        key_down_previous[i] = key_down_current[i];
}

    for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
    {
        state->mouse_down[i] = mouse_down_current[i];
        state->mouse_pressed[i] = mouse_down_current[i] && !mouse_down_previous[i];
        state->mouse_released[i] = !mouse_down_current[i] && mouse_down_previous[i];

        mouse_down_previous[i] = mouse_down_current[i];
    }

    state->mouse_x = mouse_x_current;
    state->mouse_y = mouse_y_current;
    state->mouse_dx = mouse_x_current - mouse_x_previous;
    state->mouse_dy = mouse_y_current - mouse_y_previous;

    mouse_x_previous = mouse_x_current;
    mouse_y_previous = mouse_y_current;
}

void InputProcessKeyEvent(Key key, bool is_down)
{
    key_down_current[key] = is_down;
}

void InputProcessMouseButtonEvent(MouseButton button, bool is_down)
{
    mouse_down_current[button] = is_down;
}

void InputProcessMouseMoveEvent(f32 x, f32 y)
{
    mouse_x_current = x;
    mouse_y_current = y;
}