#include "platform.h"
#include "core/input.h"

#include <stdio.h>
#include <SDL3/SDL.h>

static SDL_Window* window;
static SDL_GLContext gl_context;
static bool should_quit = false;
static f64 timer_frequency;

static Key SDLScancodeToKey(s32 scancode)
{
    switch (scancode)
    {
        case SDL_SCANCODE_W:      return KEY_W;
        case SDL_SCANCODE_A:      return KEY_A;
        case SDL_SCANCODE_S:      return KEY_S;
        case SDL_SCANCODE_D:      return KEY_D;
        case SDL_SCANCODE_E:      return KEY_E;
        case SDL_SCANCODE_UP:     return KEY_UP;
        case SDL_SCANCODE_DOWN:   return KEY_DOWN;
        case SDL_SCANCODE_LEFT:   return KEY_LEFT;
        case SDL_SCANCODE_RIGHT:  return KEY_RIGHT;
        case SDL_SCANCODE_SPACE:  return KEY_SPACE;
        case SDL_SCANCODE_ESCAPE: return KEY_ESCAPE;
        case SDL_SCANCODE_TAB:    return KEY_TAB;
        default:                  return KEY_UNKNOWN;
    }
}

static MouseButton SDLButtonToMouseButton(s32 button)
{
    switch (button)
    {
        case SDL_BUTTON_LEFT:   return MOUSE_BUTTON_LEFT;
        case SDL_BUTTON_RIGHT:  return MOUSE_BUTTON_RIGHT;
        case SDL_BUTTON_MIDDLE: return MOUSE_BUTTON_MIDDLE;
        default:                return MOUSE_BUTTON_COUNT;
    }
}

static void PlatformProcessKeyEvent(s32 scancode, bool is_down)
{
    Key key = SDLScancodeToKey(scancode);

    if (key != KEY_UNKNOWN)
    {
        InputProcessKeyEvent(key, is_down);
    }
}

static void PlatformProcessMouseButtonEvent(s32 button, bool is_down)
{
    MouseButton mouse_button = SDLButtonToMouseButton(button);

    if (mouse_button != MOUSE_BUTTON_COUNT)
    {
        InputProcessMouseButtonEvent(mouse_button, is_down);
    }
}

static void PlatformProcessMouseMoveEvent(f32 x, f32 y, f32 dx, f32 dy)
{
    InputProcessMouseMoveEvent(x, y, dx, dy);
}

void PlatformInit()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Gunman", 1280, 720, SDL_WINDOW_OPENGL);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    //SDL_SetWindowMouseGrab(window, true);
    SDL_SetWindowRelativeMouseMode(window, true);

    timer_frequency = (f64)SDL_GetPerformanceFrequency();
}

void PlatformShutdown()
{
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

f64 PlatformGetTime()
{
    f64 result = SDL_GetPerformanceCounter() / timer_frequency;
    return result;
}

void PlatformSwapBuffers()
{
    SDL_GL_SwapWindow(window);
}

void PlatformPollEvents()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
            case SDL_EVENT_QUIT: {
                should_quit = true;
            } break;

            case SDL_EVENT_KEY_DOWN: {
                PlatformProcessKeyEvent(ev.key.scancode, true);
            } break;

            case SDL_EVENT_KEY_UP: {
                PlatformProcessKeyEvent(ev.key.scancode, false);
            } break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:{
                PlatformProcessMouseButtonEvent(ev.button.button, true);
            } break;

            case SDL_EVENT_MOUSE_BUTTON_UP: {
                PlatformProcessMouseButtonEvent(ev.button.button, false);
            } break;

            case SDL_EVENT_MOUSE_MOTION:{
                PlatformProcessMouseMoveEvent(ev.motion.x, ev.motion.y,
                                              ev.motion.xrel, ev.motion.yrel);
            } break;
        }
    }
}

bool PlatformShouldQuit()
{
    return should_quit;
}

void* PlatformAllocateMemory(u64 size)
{
    return malloc(size);
}

void PlatformFreeMemory(void* memory)
{
    free(memory);
}

void* PlatformCopyMemory(void* dest, const void* src, u64 size)
{
    return memcpy(dest, src, size);
}

void* PlatformClearMemory(void* dest, u64 size)
{
    return memset(dest, 0, size);
}

void PlatformLog(const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}