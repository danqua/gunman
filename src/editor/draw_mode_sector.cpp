#include "draw_mode_sector.h"

#include "core/input.h"
#include "renderer/renderer_2d.h"
#include "editor.h"

void DrawModeSector_Enter(EditorState* state) {
    state->mode = EditorDrawMode_Sector;
    state->points.Add(glm::vec2(state->snappedPos));
}

void DrawModeSector_Exit(EditorState* state) {
    
}

void DrawModeSector_Update(EditorState* state, f32 dt) {
    if (IsMouseButtonPressed(MouseButton_Left)) {
        state->points.Add(glm::vec2(state->snappedPos));
    }
}

void DrawModeSector_Render(EditorState* state) {
    glm::vec2 segmentPos = glm::vec2(state->snappedPos);
    for (s32 i = 0; i < state->points.size - 1; ++i) {
        glm::vec2 p1 = state->points.data[i];
        glm::vec2 p2 = state->points.data[i + 1];
        Renderer2D_DrawLine(p1, p2, COLOR_WHITE);
        Renderer2D_DrawRect(p1 - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
    }

    glm::vec2 lastPoint = state->points.data[state->points.size - 1];
    Renderer2D_DrawLine(lastPoint, segmentPos, COLOR_WHITE);
    Renderer2D_DrawRect(lastPoint - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
}