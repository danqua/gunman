#include "input.h"
#include <string.h>

static bool key_down_current[KEY_COUNT];
static bool key_down_previous[KEY_COUNT];

static bool mouse_down_current[MOUSE_BUTTON_COUNT];
static bool mouse_down_previous[MOUSE_BUTTON_COUNT];

static f32 mouse_x;
static f32 mouse_y;
static f32 mouse_dx;
static f32 mouse_dy;

void InputInit()
{
    memset(key_down_current, 0, sizeof(key_down_current));
    memset(key_down_previous, 0, sizeof(key_down_previous));

    memset(mouse_down_current, 0, sizeof(mouse_down_current));
    memset(mouse_down_previous, 0, sizeof(mouse_down_previous));

    mouse_x = 0.0f;
    mouse_y = 0.0f;
    mouse_dx = 0.0f;
    mouse_dy = 0.0f;
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

    state->mouse_x = mouse_x;
    state->mouse_y = mouse_y;
    state->mouse_dx = mouse_dx;
    state->mouse_dy = mouse_dy;

    mouse_dx = 0;
    mouse_dy = 0;
}

void InputProcessKeyEvent(Key key, bool is_down)
{
    key_down_current[key] = is_down;
}

void InputProcessMouseButtonEvent(MouseButton button, bool is_down)
{
    mouse_down_current[button] = is_down;
}

void InputProcessMouseMoveEvent(f32 x, f32 y, f32 dx, f32 dy)
{
    mouse_x = x;
    mouse_y = y;
    mouse_dx = dx;
    mouse_dy = dy;
}