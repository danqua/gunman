#pragma once
#include "core/types.h"
#include "core/input.h"
#include "core/math.h"
#include "core/time.h"

#include "render/camera.h"

#include "game/entity.h"
#include "game/level.h"

struct GameState
{
    Camera camera;
    Entity* player;
    Level level;
};

struct Game
{
    Arena permanent_arena;

    GameState state;
    Time time;
    InputState input;
};

// Initializes the game state.
void GameInit(Game* game);

// Cleans up the game state.
void GameRun(Game* game);

// Updates the game state.
void GameUpdate(Game* game, f32 dt);

// Fixed update for the game state, used for physics.
void GameFixedUpdate(Game* game, f32 dt);

// Renders the game state.
void GameRender(Game* game);