#include "platform/platform.h"
#include "core/input.h"
#include "core/time.h"
#include "core/memory.h"

#include <stdio.h>

struct Game
{
    Arena* permanent_arena;
    Arena* transient_arena;

    Time time;
    InputState input;
};

void GameFixedUpdate(Game* game, f32 fixed_dt)
{
    
}

void GameUpdate(Game* game, f32 dt)
{
}

void GameRender(Game* game)
{

}

int main(int argc, char** argv)
{
    Game game = {};
    InputInit();
    TimeInit(&game.time);

    PlatformInit();

    while (!PlatformShouldQuit())
    {
        PlatformPollEvents();
        
        f32 now = PlatformGetTime();
        TimeUpdate(&game.time, now);

        InputUpdate(&game.input);

        // Run physics/game logic in fixed steps.
        while (TimeShouldStep(&game.time))
        {
            GameFixedUpdate(&game, game.time.fixed_dt);
        }

        // Smooth stuff
        GameUpdate(&game, game.time.delta_time);

        GameRender(&game);

        PlatformSwapBuffers();
    }

    PlatformShutdown();
    return 0;
}