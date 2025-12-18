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
    EditorDrawMode_Sector,
    EditorDrawMode_MoveVertex
};

enum EditModeType {
    EditMode_None,
    EditMode_DrawWalls,
    EditMode_DragVertex
};

struct EditMode {
    EditModeType type;
    LineSegment* activeSeg;
    bool firstVertex;
};

#define MAX_MAP_VERTICES 4096
#define MAX_MAP_SEGMENTS 8192

struct MapVertex {
    s32 x;
    s32 y;
};

struct MapLineSegment {
    s32 v1;
    s32 v2;
};

struct MapEdge {
    s32 segment;
    bool reversed;
};

struct EditorState {
    Camera2D camera;


    EditMode editMode;


    u16 gridSize;

    glm::vec2 dragAnchor;
    bool isDragging;

    EditorDrawMode mode;






    std::array<Sector, MAX_SECTORS> sectors;
    u64 sectorCount;

    std::vector<glm::ivec2> points;
    std::vector<LineSegment> vsegments;
    std::unordered_map<s32, std::vector<Edge>> sectorEdgeMap;



    MapVertex vertices[MAX_MAP_VERTICES];
    u32 vertexCount;

    MapLineSegment lineSegments[MAX_MAP_SEGMENTS];
    u32 lineSegmentCount;

};

void Editor_Init();
void Editor_UpdateAndRender(f32 dt);

void Editor_ChangeState(EditorState* state, EditorDrawMode newMode);

bool PointOnSegment(glm::vec2 p, glm::vec2 v1, glm::vec2 v2);

glm::ivec2 SnapToGrid(const EditorState* state, glm::vec2 worldPos);
glm::ivec2 GetSnappedMousePosition(const EditorState* state);

