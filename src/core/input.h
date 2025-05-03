#pragma once
#include "core/types.h"
#include <glm/glm.hpp>

enum Key {
    Key_Unknown = 0,

    // Letters
    Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G,
    Key_H, Key_I, Key_J, Key_K, Key_L, Key_M, Key_N,
    Key_O, Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U,
    Key_V, Key_W, Key_X, Key_Y, Key_Z,

    // Numbers
    Key_0, Key_1, Key_2, Key_3, Key_4,
    Key_5, Key_6, Key_7, Key_8, Key_9,

    // Function keys
    Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6,
    Key_F7, Key_F8, Key_F9, Key_F10, Key_F11, Key_F12,

    // Arrows
    Key_Up, Key_Down, Key_Left, Key_Right,

    // Control keys
    Key_Escape,
    Key_Enter,
    Key_Tab,
    Key_Backspace,
    Key_Space,
    Key_LeftShift,
    Key_RightShift,
    Key_LeftCtrl,
    Key_RightCtrl,
    Key_LeftAlt,
    Key_RightAlt,

    // Misc
    Key_Insert,
    Key_Delete,
    Key_Home,
    Key_End,
    Key_PageUp,
    Key_PageDown,

    Key_Count
};

enum MouseButton {
    MouseButton_Left = 0,
    MouseButton_Right,
    MouseButton_Middle,
    MouseButton_Count
};

void Input_Init();
void Input_Shutdown();
void Input_NextFrame();
void Input_ProcessKeyEvent(Key key, b32 down);
void Input_ProcessMouseMoveEvent(f32 x, f32 y, f32 xrel, f32 yrel);

b32 IsKeyDown(Key key);
b32 IsKeyPressed(Key key);
b32 IsKeyReleased(Key key);

glm::vec2 GetRelativeMousePosition();