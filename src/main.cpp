#include <SDL3/SDL.h>
#include "core/input.h"

int main(int argc, char** argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Gunman", 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);

    bool running = true;

    InputInit();

    while (running)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (ev.type == SDL_EVENT_KEY_DOWN)
            {
                switch (ev.key.scancode)
                {
                    case SDL_SCANCODE_W: InputProcessKeyEvent(KEY_W, true); break;
                    case SDL_SCANCODE_A: InputProcessKeyEvent(KEY_A, true); break;
                    case SDL_SCANCODE_S: InputProcessKeyEvent(KEY_S, true); break;
                    case SDL_SCANCODE_D: InputProcessKeyEvent(KEY_D, true); break;

                    case SDL_SCANCODE_UP: InputProcessKeyEvent(KEY_UP, true); break;
                    case SDL_SCANCODE_DOWN: InputProcessKeyEvent(KEY_DOWN, true); break;
                    case SDL_SCANCODE_LEFT: InputProcessKeyEvent(KEY_LEFT, true); break;
                    case SDL_SCANCODE_RIGHT: InputProcessKeyEvent(KEY_RIGHT, true); break;

                    case SDL_SCANCODE_SPACE: InputProcessKeyEvent(KEY_SPACE, true); break;
                    case SDL_SCANCODE_ESCAPE: InputProcessKeyEvent(KEY_ESCAPE, true); break;
                }
            }
            else if (ev.type == SDL_EVENT_KEY_UP)
            {
                switch (ev.key.scancode)
                {
                    case SDL_SCANCODE_W: InputProcessKeyEvent(KEY_W, false); break;
                    case SDL_SCANCODE_A: InputProcessKeyEvent(KEY_A, false); break;
                    case SDL_SCANCODE_S: InputProcessKeyEvent(KEY_S, false); break;
                    case SDL_SCANCODE_D: InputProcessKeyEvent(KEY_D, false); break;

                    case SDL_SCANCODE_UP: InputProcessKeyEvent(KEY_UP, false); break;
                    case SDL_SCANCODE_DOWN: InputProcessKeyEvent(KEY_DOWN, false); break;
                    case SDL_SCANCODE_LEFT: InputProcessKeyEvent(KEY_LEFT, false); break;
                    case SDL_SCANCODE_RIGHT: InputProcessKeyEvent(KEY_RIGHT, false); break;

                    case SDL_SCANCODE_SPACE: InputProcessKeyEvent(KEY_SPACE, false); break;
                    case SDL_SCANCODE_ESCAPE: InputProcessKeyEvent(KEY_ESCAPE, false); break;
                }
            }
            else if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                switch (ev.button.button)
                {
                    case SDL_BUTTON_LEFT: InputProcessMouseButtonEvent(MOUSE_BUTTON_LEFT, true); break;
                    case SDL_BUTTON_RIGHT: InputProcessMouseButtonEvent(MOUSE_BUTTON_RIGHT, true); break;
                    case SDL_BUTTON_MIDDLE: InputProcessMouseButtonEvent(MOUSE_BUTTON_MIDDLE, true); break;
                }
            }
            else if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                switch (ev.button.button)
                {
                    case SDL_BUTTON_LEFT: InputProcessMouseButtonEvent(MOUSE_BUTTON_LEFT, false); break;
                    case SDL_BUTTON_RIGHT: InputProcessMouseButtonEvent(MOUSE_BUTTON_RIGHT, false); break;
                    case SDL_BUTTON_MIDDLE: InputProcessMouseButtonEvent(MOUSE_BUTTON_MIDDLE, false); break;
                }
            }
            else if (ev.type == SDL_EVENT_MOUSE_MOTION)
            {
                InputProcessMouseMoveEvent(ev.motion.x, ev.motion.y);
            }
        }

        static Uint64 last_time = SDL_GetPerformanceCounter();
        Uint64 current_time = SDL_GetPerformanceCounter();
        float delta_time = (f32)(current_time - last_time) / SDL_GetPerformanceFrequency();
        last_time = current_time;

        InputState input;
        InputUpdate(&input);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}