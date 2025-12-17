#include "editor.h"
#include "core/platform.h"
#include "core/input.h"
#include "core/containers.h"
#include "renderer/camera.h"
#include "renderer/renderer_2d.h"

#include "draw_mode_sector.h"

#define MAX_LEVEL_WIDTH 4096
#define MAX_LEVEL_HEIGHT 4096

static EditorState editorState;

void DrawCursor() {
    glm::vec2 mousePos = editorState.mousePosition;
    f32 halfSize = 15.0f;
    Renderer2D_DrawLine(mousePos - glm::vec2(halfSize, 0.0f), mousePos + glm::vec2(halfSize, 0.0f), COLOR_RED);
    Renderer2D_DrawLine(mousePos - glm::vec2(0.0f, halfSize), mousePos + glm::vec2(0.0f, halfSize), COLOR_RED);
}

void Editor_Init() {
    s32 windowWidth = Platform_GetWindowWidth();
    s32 windowHeight = Platform_GetWindowHeight();
    Platform_SetMouseCaptured(true);
    Renderer2D_SetSize((f32)windowWidth, (f32)windowHeight);

    editorState.zoomLevel = 1.0f;
    editorState.cameraPosition = glm::vec2(0.0f, 0.0f);

}

void Editor_UpdateAndRender(f32 dt) {
    editorState.enablePan = false;

    if (IsMouseButtonDown(MouseButton_Middle)) {
        editorState.enablePan = true;
    }

    const f32 oldZoomLevel = editorState.zoomLevel;
    editorState.zoomLevel += GetMouseWheelDelta();
    if (oldZoomLevel != editorState.zoomLevel) {
        editorState.zoomLevel = glm::clamp(editorState.zoomLevel, 1.0f, 5.0f);
        f32 windowWidth = (f32)Platform_GetWindowWidth();
        f32 windowHeight = (f32)Platform_GetWindowHeight();
        Renderer2D_SetSize(windowWidth / editorState.zoomLevel, windowHeight / editorState.zoomLevel);

        // TODO: Adjust camera position to zoom towards mouse position
    }

    editorState.mousePosition = GetMousePosition();

    // Screen to world coordinates
    glm::vec2 worldMousePos = editorState.mousePosition / editorState.zoomLevel + editorState.cameraPosition;
    editorState.snappedPos = glm::ivec2(
        (s32)((worldMousePos.x + 16.0f) / 32.0f) * 32,
        (s32)((worldMousePos.y + 16.0f) / 32.0f) * 32
    );

    // Handle panning
    if (editorState.enablePan) {
        glm::vec2 mouseDelta = GetDeltaMousePosition();
        editorState.cameraPosition -= mouseDelta / editorState.zoomLevel;
        Renderer2D_SetViewOffset(editorState.cameraPosition.x, editorState.cameraPosition.y);
    }


    if (editorState.mode == EditorDrawMode_None) {
        if (IsMouseButtonPressed(MouseButton_Left)) {
            DrawModeSector_Enter(&editorState);
        }
    }
    

    // Draw grid
    Renderer2D_BeginFrame();

    s32 windowWidth = Platform_GetWindowWidth();
    s32 windowHeight = Platform_GetWindowHeight();

    for (s32 y = 0; y <= windowHeight; y += 32) {
        Renderer2D_DrawLine(glm::vec2(0.0f, (f32)y), glm::vec2((f32)windowWidth, (f32)y), COLOR_DARK_GRAY);
    }

    for (s32 x = 0; x <= windowWidth; x += 32) {
        Renderer2D_DrawLine(glm::vec2((f32)x, 0.0f), glm::vec2((f32)x, (f32)windowHeight), COLOR_DARK_GRAY);
    }


    switch (editorState.mode) {
        case EditorDrawMode_None: {
            DrawModeSector_Enter(&editorState);
        } break;

        case EditorDrawMode_Sector: {
            DrawModeSector_Update(&editorState, dt);
            DrawModeSector_Render(&editorState);
        } break;
    }

    for (s32 i = 0; i < editorState.segments.size; ++i) {
        LineSegment* segment = &editorState.segments.data[i];
        Renderer2D_DrawLine(segment->v1, segment->v2, COLOR_WHITE);
        Renderer2D_DrawRect(segment->v1 - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
        Renderer2D_DrawRect(segment->v2 - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
    }

    Renderer2D_DrawRect(glm::vec2(editorState.snappedPos) - glm::vec2(5.0f), glm::vec2(10.0f, 10.0f), COLOR_WHITE);

    DrawCursor();
    Renderer2D_EndFrame();
}