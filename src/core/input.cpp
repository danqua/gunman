#include "input.h"
#include "platform.h"

bool currentKeys[Key_Count];
bool previousKeys[Key_Count];

glm::vec2 mousePosition;
glm::vec2 relativeMousePosition;

void Input_Init() {
    for (size_t i = 0; i < Key_Count; ++i) {
        currentKeys[i] = false;
        previousKeys[i] = false;
    }
}

void Input_Shutdown() {
    // Do nothing
}

void Input_NextFrame() {
    Platform_CopyMemory(previousKeys, currentKeys, sizeof(currentKeys));
    relativeMousePosition = glm::vec2(0.0f);
}

void Input_ProcessKeyEvent(Key key, bool down) {
    if (key >= Key_Count) {
        return;
    }

    size_t index = (size_t)key;
    currentKeys[index] = down;
}

void Input_ProcessMouseMoveEvent(f32 x, f32 y, f32 xrel, f32 yrel) {
    mousePosition.x = x;
    mousePosition.y = y;

    relativeMousePosition.x = xrel;
    relativeMousePosition.y = yrel;
}

bool IsKeyDown(Key key) {
    if (key >= Key_Count) {
        return false;
    }

    size_t index = (size_t)key;
    return currentKeys[index];
}

bool IsKeyPressed(Key key) {
    if (key >= Key_Count) {
        return false;
    }

    size_t index = (size_t)key;
    return currentKeys[index] && !previousKeys[index];
}

bool IsKeyReleased(Key key) {
    if (key >= Key_Count) {
        return false;
    }

    size_t index = (size_t)key;
    return !currentKeys[index] && previousKeys[index];
}

glm::vec2 GetMousePosition() {
    return mousePosition;
}

glm::vec2 GetDeltaMousePosition() {
    return relativeMousePosition;
}