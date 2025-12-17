#pragma once
#include "core/types.h"

struct EditorState;

void DrawModeSector_Enter(EditorState* state);
void DrawModeSector_Exit(EditorState* state);
void DrawModeSector_Update(EditorState* state, f32 dt);
void DrawModeSector_Render(EditorState* state);