#pragma once
#include "core/types.h"

enum Key
{
    KEY_UNKNOWN = 0,
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_E,

    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    
    KEY_SPACE,
    KEY_ESCAPE,

    KEY_TAB,

    KEY_COUNT
};

enum MouseButton
{
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_COUNT
};

struct InputState
{
    bool key_down[KEY_COUNT];      // Is key currently down.
    bool key_pressed[KEY_COUNT];   // Was key pressed this frame.
    bool key_released[KEY_COUNT];  // Was key released this frame.

    bool mouse_down[MOUSE_BUTTON_COUNT];        // Is mouse button currently down.
    bool mouse_pressed[MOUSE_BUTTON_COUNT];     // Was mouse button pressed this frame.
    bool mouse_released[MOUSE_BUTTON_COUNT];    // Was mouse button released this frame.

    f32 mouse_x;    // Mouse x position.
    f32 mouse_y;    // Mouse y position.
    f32 mouse_dx;   // Mouse x delta.
    f32 mouse_dy;   // Mouse y delta;
};

// Initialize input system
void InputInit();

// Update input state for the current frame
void InputUpdate(InputState* state);

// ----------------------------------------------------------------
// Platform specific functions called by the platform layer
// ----------------------------------------------------------------

// Process key event
void InputProcessKeyEvent(Key key, bool is_down);

// Process mouse button event
void InputProcessMouseButtonEvent(MouseButton button, bool is_down);

// Process mouse move event
void InputProcessMouseMoveEvent(f32 x, f32 y, f32 dx, f32 dy);