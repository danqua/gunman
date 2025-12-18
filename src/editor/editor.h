#pragma once
#include "core/types.h"
#include "core/containers.h"
#include "renderer/renderer_2d.h"
#include "game/map.h"

#include <array>
#include <vector>
#include <unordered_map>

#define MAX_SECTORS 1024

enum EditorDrawMode {
    EditorDrawMode_None,
    EditorDrawMode_Sector
};


struct EditorState {
    Camera2D camera;

    u16 gridSize;

    glm::vec2 dragAnchor;
    bool isDragging;

    EditorDrawMode mode;

    std::array<Sector, MAX_SECTORS> sectors;
    u64 sectorCount;

    std::vector<glm::ivec2> points;
    std::vector<LineSegment> segments;
    std::unordered_map<s32, std::vector<Edge>> sectorEdgeMap;
};

void Editor_Init();
void Editor_UpdateAndRender(f32 dt);

glm::ivec2 Editor_SnapToGrid(const EditorState* state, const glm::vec2& position);
void Editor_ChangeState(EditorState* state, EditorDrawMode newMode);

bool PointOnSegment(glm::vec2 p, glm::vec2 v1, glm::vec2 v2);

glm::ivec2 SnapToGrid(const EditorState* state, glm::vec2 worldPos);
glm::ivec2 GetSnappedMousePosition(const EditorState* state);