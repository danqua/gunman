#include <SDL3/SDL.h>

static SDL_Window* window;
static SDL_Renderer* renderer;


int main(int argc, char** argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }

    window = SDL_CreateWindow("IronVoidEd", 1280, 720, 0);
    if (!window) {
        SDL_Quit();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, 0);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    bool running = true;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw grid
        int gridSize = 32;
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        
        int rows = windowHeight / gridSize;
        int cols = windowWidth / gridSize;

        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
        
        for (int y = 0; y < rows; ++y) {
            SDL_RenderLine(renderer, 0.0f, y * (float)gridSize, windowWidth, y * (float)gridSize);
        }

        for (int x = 0; x < cols; ++x) {
            SDL_RenderLine(renderer, x * (float)gridSize, 0.0f, x * (float)gridSize, windowHeight);
        }


        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}