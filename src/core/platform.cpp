#include "platform.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

static SDL_Window* window;
static SDL_GLContext glContext;
static b32 windowShouldClose;

static Key ConvertScancodeToKey(SDL_Scancode code) {
    switch (code) {
        // Letters
        case SDL_SCANCODE_A: return Key_A;
        case SDL_SCANCODE_B: return Key_B;
        case SDL_SCANCODE_C: return Key_C;
        case SDL_SCANCODE_D: return Key_D;
        case SDL_SCANCODE_E: return Key_E;
        case SDL_SCANCODE_F: return Key_F;
        case SDL_SCANCODE_G: return Key_G;
        case SDL_SCANCODE_H: return Key_H;
        case SDL_SCANCODE_I: return Key_I;
        case SDL_SCANCODE_J: return Key_J;
        case SDL_SCANCODE_K: return Key_K;
        case SDL_SCANCODE_L: return Key_L;
        case SDL_SCANCODE_M: return Key_M;
        case SDL_SCANCODE_N: return Key_N;
        case SDL_SCANCODE_O: return Key_O;
        case SDL_SCANCODE_P: return Key_P;
        case SDL_SCANCODE_Q: return Key_Q;
        case SDL_SCANCODE_R: return Key_R;
        case SDL_SCANCODE_S: return Key_S;
        case SDL_SCANCODE_T: return Key_T;
        case SDL_SCANCODE_U: return Key_U;
        case SDL_SCANCODE_V: return Key_V;
        case SDL_SCANCODE_W: return Key_W;
        case SDL_SCANCODE_X: return Key_X;
        case SDL_SCANCODE_Y: return Key_Y;
        case SDL_SCANCODE_Z: return Key_Z;

            // Numbers
        case SDL_SCANCODE_0: return Key_0;
        case SDL_SCANCODE_1: return Key_1;
        case SDL_SCANCODE_2: return Key_2;
        case SDL_SCANCODE_3: return Key_3;
        case SDL_SCANCODE_4: return Key_4;
        case SDL_SCANCODE_5: return Key_5;
        case SDL_SCANCODE_6: return Key_6;
        case SDL_SCANCODE_7: return Key_7;
        case SDL_SCANCODE_8: return Key_8;
        case SDL_SCANCODE_9: return Key_9;

            // Function keys
        case SDL_SCANCODE_F1:  return Key_F1;
        case SDL_SCANCODE_F2:  return Key_F2;
        case SDL_SCANCODE_F3:  return Key_F3;
        case SDL_SCANCODE_F4:  return Key_F4;
        case SDL_SCANCODE_F5:  return Key_F5;
        case SDL_SCANCODE_F6:  return Key_F6;
        case SDL_SCANCODE_F7:  return Key_F7;
        case SDL_SCANCODE_F8:  return Key_F8;
        case SDL_SCANCODE_F9:  return Key_F9;
        case SDL_SCANCODE_F10: return Key_F10;
        case SDL_SCANCODE_F11: return Key_F11;
        case SDL_SCANCODE_F12: return Key_F12;

            // Arrow keys
        case SDL_SCANCODE_UP:    return Key_Up;
        case SDL_SCANCODE_DOWN:  return Key_Down;
        case SDL_SCANCODE_LEFT:  return Key_Left;
        case SDL_SCANCODE_RIGHT: return Key_Right;

            // Control keys
        case SDL_SCANCODE_ESCAPE:     return Key_Escape;
        case SDL_SCANCODE_RETURN:     return Key_Enter;
        case SDL_SCANCODE_TAB:        return Key_Tab;
        case SDL_SCANCODE_BACKSPACE:  return Key_Backspace;
        case SDL_SCANCODE_SPACE:      return Key_Space;
        case SDL_SCANCODE_LSHIFT:     return Key_LeftShift;
        case SDL_SCANCODE_RSHIFT:     return Key_RightShift;
        case SDL_SCANCODE_LCTRL:      return Key_LeftCtrl;
        case SDL_SCANCODE_RCTRL:      return Key_RightCtrl;
        case SDL_SCANCODE_LALT:       return Key_LeftAlt;
        case SDL_SCANCODE_RALT:       return Key_RightAlt;

            // Misc keys
        case SDL_SCANCODE_INSERT:    return Key_Insert;
        case SDL_SCANCODE_DELETE:    return Key_Delete;
        case SDL_SCANCODE_HOME:      return Key_Home;
        case SDL_SCANCODE_END:       return Key_End;
        case SDL_SCANCODE_PAGEUP:    return Key_PageUp;
        case SDL_SCANCODE_PAGEDOWN:  return Key_PageDown;

        default: return Key_Unknown;
    }
}


void Platform_Assert(b32 condition, const char* message, ...)
{
    if (!condition)
    {
        va_list args;
        va_start(args, message);
        vfprintf(stderr, message, args);
        va_end(args);
        abort();
    }
}

void* Platform_Alloc(u64 size)
{
    return malloc(size);
}

void Platform_Free(void* memory)
{
    free(memory);
}

void* Platform_ClearMemory(void* memory, u64 size)
{
    return memset(memory, 0, size);
}

void* ClearMemory(void* ptr, u64 size)
{
    return memset(ptr, 0, size);
}

void* Platform_FillMemory(void* memory, s32 value, u64 size)
{
    return memset(memory, value, size);
}

void* Platform_CopyMemory(void* dest, const void* src, u64 size)
{
    return memcpy(dest, src, size);
}

b32 Platform_InitWindow(const char* title, s32 width, s32 height)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(window);
    
    SDL_GL_SetSwapInterval(1);

    SDL_SetWindowRelativeMouseMode(window, true);

    return true;
}

void Platform_CloseWindow()
{
    windowShouldClose = true;
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

s32 Platform_GetWindowWidth()
{
    s32 width;
    SDL_GetWindowSize(window, &width, NULL);
    return width;
}

s32 Platform_GetWindowHeight()
{
    s32 height;
    SDL_GetWindowSize(window, NULL, &height);
    return height;
}

b32 Platform_WindowShouldClose()
{
    return windowShouldClose;
}

void Platform_PollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            windowShouldClose = true;
            break;
            
            case SDL_EVENT_KEY_DOWN: {
                Input_ProcessKeyEvent(ConvertScancodeToKey(event.key.scancode), true);
            } break;

            case SDL_EVENT_KEY_UP: {
                Input_ProcessKeyEvent(ConvertScancodeToKey(event.key.scancode), false);
            } break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            // Handle mouse button down event
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            // Handle mouse button up event
            break;

        case SDL_EVENT_MOUSE_MOTION:
            Input_ProcessMouseMoveEvent(
                event.motion.x,
                event.motion.y,
                event.motion.xrel,
                event.motion.yrel
            );
            break;
        }
    }
}

void Platform_SwapBuffers()
{
    SDL_GL_SwapWindow(window);
}

f64 Platform_GetTime()
{
    u64 counter = SDL_GetPerformanceCounter();
    return (f64)counter / (f64)SDL_GetPerformanceFrequency();
}

const char* TextFormat(const char* format, ...)
{
    static char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, format, args);
    va_end(args);
    return buffer;
}

FileHandle Platform_OpenFile(const char* filename, FileMode mode)
{
    const char* modeIdentifier[] = { "rb", "wb" };
    FILE* fp = {};
    fopen_s(&fp, filename, modeIdentifier[mode]);
    fseek(fp, 0, SEEK_END);
    u64 size = ftell(fp);
    rewind(fp);
 
    FileHandle result = {};
    result.handle = fp;
    result.size = size;

    return result;
}

void Platform_CloseFile(FileHandle* fileHandle)
{
    fclose((FILE*)fileHandle->handle);
    *fileHandle = {};
}

void Platform_ReadDataFromFile(FileHandle* fileHandle, void* buffer, u64 size)
{
    if (fileHandle->size < size)
    {
        return;
    }

    fread_s(buffer, fileHandle->size, 1, size, (FILE*)fileHandle->handle);
}


void Platform_LogInfo(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}

void Platform_LogError(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

}

void Platform_LogWarning(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}