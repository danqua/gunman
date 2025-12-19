#pragma once
#include "core/types.h"
#include "core/containers.h"
#include "core/memory.h"
#include "renderer/renderer_2d.h"
#include "game/map.h"

#include <array>
#include <vector>
#include <unordered_map>


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
#define MAP_MAP_WALLS 8192
#define MAP_MAX_SECTOR_GROUPS 256
#define MAP_MAX_INNER_SECTORS 32
#define MAP_MAX_SECTORS 1024

#define MAP_MAX_GROUP_WALLS 128


#define MAX_MAP_SEGMENTS 8192

#define MAP_MAX_TRANSIENT_POINTS 128

struct MapVertex {
    s32 x;
    s32 y;
};

enum SectorType {
    SectorType_Outer,
    SectorType_Inner
};

struct MapSector;
struct MapWall;

struct MapSectorGroup {
    MapSector* sector;
    MapWall* walls[MAP_MAX_GROUP_WALLS];
    u32 wallCount;
    SectorType type;
};

struct MapSector {
    MapSectorGroup* groups[MAP_MAX_INNER_SECTORS];
    u32 groupCount;
    f32 floorHeight;
    f32 ceilingHeight;
};

struct MapWall {
    MapVertex* v1;
    MapVertex* v2;
    MapSector* sector;
    MapSectorGroup* group;
    MapWall* twin;
};

struct EditorState {
    Camera2D camera;


    EditMode editMode;


    u16 gridSize;

    glm::vec2 dragAnchor;
    bool isDragging;

    EditorDrawMode mode;

    Pool vertexPool;
    Pool wallPool;
    Pool groupPool;
    Pool sectorPool;

    MapVertex* vertices[MAX_MAP_VERTICES];
    u32 vertexCount;

    MapWall* walls[MAP_MAP_WALLS];
    u32 wallCount;

    MapSectorGroup* sectorGroups[MAP_MAX_SECTOR_GROUPS];
    u32 sectorGroupCount;

    MapSector* sectors[MAP_MAX_SECTORS];
    u32 sectorCount;

    bool dragVertex;
    MapVertex* activeVertex;

    MapWall* closestWall;
    glm::vec2 closestPointOnWall;




    glm::ivec2 tVertices[MAP_MAX_TRANSIENT_POINTS];
    u64 tVertexCount;


    std::array<Sector, MAP_MAX_SECTORS> arrsectors;
    u64 arrSectorCount;

    std::vector<glm::ivec2> vpoints;
    std::vector<LineSegment> vsegments;
    std::unordered_map<s32, std::vector<Edge>> sectorEdgeMap;
};

void Editor_Init();
void Editor_UpdateAndRender(f32 dt);

void Editor_ChangeState(EditorState* state, EditorDrawMode newMode);

bool PointOnSegment(glm::vec2 p, glm::vec2 v1, glm::vec2 v2);

glm::ivec2 SnapToGrid(const EditorState* state, glm::vec2 worldPos);
glm::ivec2 GetSnappedMousePosition(const EditorState* state);

