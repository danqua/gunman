#include "input.h"
#include "platform.h"

b32 currentKeys[Key_Count];
b32 previousKeys[Key_Count];

glm::vec2 mousePosition;
glm::vec2 relativeMousePosition;

void Input_Init()
{
    for (size_t i = 0; i < Key_Count; ++i)
    {
        currentKeys[i] = false;
        previousKeys[i] = false;
    }
}

void Input_Shutdown()
{

}

void Input_NextFrame()
{
    Platform_CopyMemory(previousKeys, currentKeys, sizeof(currentKeys));
    relativeMousePosition = glm::vec2(0.0f);
}

void Input_ProcessKeyEvent(Key key, b32 down)
{
    if (key >= Key_Count)
    {
        return;
    }

    size_t index = (size_t)key;
    currentKeys[index] = down;
}

void Input_ProcessMouseMoveEvent(f32 x, f32 y, f32 xrel, f32 yrel)
{
    mousePosition.x = x;
    mousePosition.y = y;

    relativeMousePosition.x = xrel;
    relativeMousePosition.y = yrel;
}

b32 IsKeyDown(Key key)
{
    if (key >= Key_Count)
    {
        return false;
    }

    size_t index = (size_t)key;
    return currentKeys[index];
}

b32 IsKeyPressed(Key key)
{
    if (key >= Key_Count)
    {
        return false;
    }

    size_t index = (size_t)key;
    return currentKeys[index] && !previousKeys[index];
}

b32 IsKeyReleased(Key key)
{
    if (key >= Key_Count)
    {
        return false;
    }

    size_t index = (size_t)key;
    return !currentKeys[index] && previousKeys[index];
}

glm::vec2 GetRelativeMousePosition()
{
    return relativeMousePosition;
}