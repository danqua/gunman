#pragma once
#include "core/types.h"
#include "core/containers.h"
#include "game/map.h"

enum EditorDrawMode {
    EditorDrawMode_None,
    EditorDrawMode_Sector
};

struct EditorState {
    glm::vec2 cameraPosition;

    glm::vec2 mousePosition;
    glm::ivec2 snappedPos;

    f32 zoomLevel;
    bool enablePan;

    EditorDrawMode mode;
    DynamicArray<glm::vec2> points;

    DynamicArray<LineSegment> segments;
    DynamicArray<Sector> sectors;
    DynamicArray<Edge> edges;
};

void Editor_Init();
void Editor_UpdateAndRender(f32 dt);