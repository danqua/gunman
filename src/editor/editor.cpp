#include "editor.h"
#include "core/platform.h"
#include "core/input.h"
#include "core/containers.h"
#include "renderer/camera.h"
#include "renderer/renderer_2d.h"
#include "core/math.h"

#include "draw_mode_sector.h"

#define MAX_LEVEL_WIDTH 4096
#define MAX_LEVEL_HEIGHT 4096

static EditorState editorState;

void DrawCursor() {
    glm::vec2 mousePos = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());
    f32 halfSize = 15.0f / editorState.camera.pixelsPerUnit;
    Renderer2D_DrawLine(mousePos - glm::vec2(halfSize, 0.0f), mousePos + glm::vec2(halfSize, 0.0f), COLOR_RED);
    Renderer2D_DrawLine(mousePos - glm::vec2(0.0f, halfSize), mousePos + glm::vec2(0.0f, halfSize), COLOR_RED);
}

void DrawVertex(const Camera2D* camera, glm::vec2 position) {
    const f32 halfSize = 5.0f / camera->pixelsPerUnit;
    Renderer2D_DrawRect(position - glm::vec2(halfSize), glm::vec2(halfSize * 2), COLOR_WHITE);
}

void DrawWall(const Camera2D* camera, glm::vec2 v1, glm::vec2 v2, Color color) {
    Renderer2D_DrawLine(v1, v2, color);
    DrawVertex(camera, v1);
    DrawVertex(camera, v2);
}

void DrawClosestPointVertex(const Camera2D* camera, glm::vec2 pos) {
    f32 halfLength = 5.0f;
    Renderer2D_DrawLine(pos + glm::vec2(-halfLength, -halfLength), pos + glm::vec2(halfLength, halfLength), Color{ 255, 81, 14, 255 });
    Renderer2D_DrawLine(pos + glm::vec2(-halfLength, halfLength), pos + glm::vec2(halfLength, -halfLength), Color{ 255, 81, 14, 255 });
}

void Editor_Init() {
    editorState.gridSize = 32;
    s32 windowWidth = Platform_GetWindowWidth();
    s32 windowHeight = Platform_GetWindowHeight();
    editorState.camera = CreateDefaultCamera2D(windowWidth, windowHeight);


    // Initialize pools
    void* verticesPoolMemory = Platform_Alloc(sizeof(MapVertex) * MAX_MAP_VERTICES);
    void* wallsPoolMemory = Platform_Alloc(sizeof(MapWall) * MAP_MAP_WALLS);
    void* groupsPoolMemory = Platform_Alloc(sizeof(MapSectorGroup) * MAP_MAX_SECTOR_GROUPS);
    void* sectorsPoolMemory = Platform_Alloc(sizeof(MapSector) * MAP_MAX_SECTORS);

    Pool_Init(&editorState.vertexPool, sizeof(MapVertex), MAX_MAP_VERTICES, verticesPoolMemory);
    Pool_Init(&editorState.wallPool, sizeof(MapWall), MAP_MAP_WALLS, wallsPoolMemory);
    Pool_Init(&editorState.groupPool, sizeof(MapSectorGroup), MAP_MAX_SECTOR_GROUPS, groupsPoolMemory);
    Pool_Init(&editorState.sectorPool, sizeof(MapSector), MAP_MAX_SECTORS, sectorsPoolMemory);
    
    Platform_SetMouseCaptured(true);
    Renderer2D_SetSize((f32)windowWidth, (f32)windowHeight);
}

void AddVertex(EditorState* state, glm::ivec2 point) {
    state->tVertices[state->tVertexCount] = point;
    state->tVertexCount++;
}

void SplitWall(EditorState* state, MapWall* wall, glm::vec2 point) {
    glm::ivec2 snappedPoint = SnapToGrid(state, point);

    MapWall* newWall = (MapWall*)Pool_Alloc(&state->wallPool);
    state->walls[state->wallCount++] = newWall;
    *newWall = *wall;

    MapVertex* newVertex = (MapVertex*)Pool_Alloc(&state->vertexPool);
    state->vertices[state->vertexCount++] = newVertex;
    newVertex->x = snappedPoint.x;
    newVertex->y = snappedPoint.y;

    wall->v2 = newVertex;
    newWall->v1 = newVertex;

    // TODO: CreateWall-function
}

void CompleteSector(EditorState* state) {
    // TODO: Check if it is an inner or outer sector

    // Create sector
    MapSector* sector = (MapSector*)Pool_Alloc(&state->sectorPool);
    sector->floorHeight = 0;
    sector->ceilingHeight = 128;
    sector->groupCount = 1;

    // Create group
    MapSectorGroup* group = (MapSectorGroup*)Pool_Alloc(&state->groupPool);
    group->sector = sector;
    group->type = SectorType_Outer;

    // Assign group to sector
    sector->groups[0] = group;

    // Add the sector and group to the state
    state->sectors[state->sectorCount++] = sector;
    state->sectorGroups[state->sectorGroupCount++] = group;

    for (s32 i = 0; i < state->tVertexCount; i++) {
        MapWall* wall = (MapWall*)Pool_Alloc(&state->wallPool);

        wall->group = group;
        wall->sector = sector;

        state->walls[state->wallCount++] = wall;
        group->walls[group->wallCount++] = wall;

        glm::ivec2 v1 = state->tVertices[i];
        glm::ivec2 v2 = state->tVertices[(i + 1) % state->tVertexCount];

        // Check if the vertex already exists
        bool v1Exists = false;
        bool v2Exists = false;
        for (s32 j = 0; j < state->vertexCount; j++) {
            MapVertex* v = state->vertices[j];
            if (!v1Exists && v->x == v1.x && v->y == v1.y) {
                wall->v1 = state->vertices[j];
                v1Exists = true;
            } else if (!v2Exists && v->x == v2.x && v->y == v2.y) {
                wall->v2 = state->vertices[j];
                v2Exists = true;
            }
        }

        // If both vertices exist, check for twin wall
        if (v1Exists && v2Exists) {
            for (s32 j = 0; j < state->wallCount; j++) {
                MapWall* existingWall = state->walls[j];
                if (existingWall->v1 == wall->v2 && existingWall->v2 == wall->v1) {
                    wall->twin = existingWall;
                    existingWall->twin = wall;
                    break;
                }
            }
        }

        // Add new vertices if they don't exist
        if (!v1Exists) {
            MapVertex* v = (MapVertex*)Pool_Alloc(&state->vertexPool);
            state->vertices[state->vertexCount++] = v;
            v->x = v1.x;
            v->y = v2.y;
            wall->v1 = v;
        }

        if (!v2Exists) {
            MapVertex* v = (MapVertex*)Pool_Alloc(&state->vertexPool);
            state->vertices[state->vertexCount++] = v;
            v->x = v2.x;
            v->y = v2.y;
            wall->v2 = v;
        }
    }
}

void Editor_UpdateAndRender(f32 dt) {
    // Handle camera panning
    {
        if (IsMouseButtonPressed(MouseButton_Middle)) {
            if (!editorState.isDragging) {
                editorState.isDragging = true;
                editorState.dragAnchor = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());
            }
        }

        if (IsMouseButtonReleased(MouseButton_Middle)) {
            editorState.isDragging = false;
        }

        if (editorState.isDragging) {
            glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());
            glm::vec2 delta = editorState.dragAnchor - worldMousePos;
            editorState.camera.center += delta;
        }
    }

    // Handle camera zooming
    {
        f32 scrollDelta = GetMouseWheelDelta();
        if (scrollDelta != 0.0f) {
            f32 zoomFactor = 1.0f + scrollDelta * 0.2f;
            Camera2D* camera = &editorState.camera;
            glm::vec2 mousePos = GetMousePosition();
            Camera2D_ZoomAtWorldPoint(camera, zoomFactor, mousePos);
        }
    }

    // Begin 2D rendering
    const Camera2D* camera = &editorState.camera;
    f32 halfWindowW = (f32)camera->viewportSize.x * 0.5f;
    f32 halfWindowH = (f32)camera->viewportSize.y * 0.5f;
    Camera orthoCamera = Camera_CreateOrthographic(-halfWindowW, halfWindowW, halfWindowH, -halfWindowH, -1.0f, 1.0f);
    glm::mat4 projection = Camera_GetProjectionMatrix(&orthoCamera);
    glm::mat4 view = Camera2D_GetViewMatrix(&editorState.camera);

    Renderer2D_BeginFrame(&projection, &view);

    // Draw grid lines
    {
        s32 gridSize = editorState.gridSize;
        s32 windowWidth = Platform_GetWindowWidth();
        s32 windowHeight = Platform_GetWindowHeight();
        s32 wh = (s32)glm::ceil((f32)windowHeight / gridSize) * gridSize;
        s32 ww = (s32)glm::ceil((f32)windowWidth / gridSize) * gridSize;

        // Horizontal lines
        for (s32 y = 0; y <= wh; y += 32) {
            Renderer2D_DrawLine(glm::vec2(0.0f, (f32)y), glm::vec2((f32)ww, (f32)y), COLOR_DARK_GRAY);
        }

        // Vertical lines
        for (s32 x = 0; x <= ww; x += 32) {
            Renderer2D_DrawLine(glm::vec2((f32)x, 0.0f), glm::vec2((f32)x, (f32)wh), COLOR_DARK_GRAY);
        }
    }


    // Draw all sectors
    for (const auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
        for (const auto& edge : edges) {
            const LineSegment& seg = editorState.vsegments[edge.seg];
            Renderer2D_DrawLine(seg.v1, seg.v2, seg.backSector != -1 ? COLOR_RED : COLOR_WHITE);
            DrawVertex(&editorState.camera, seg.v1);
            DrawVertex(&editorState.camera, seg.v2);
        }
    }

    switch (editorState.mode) {
        case EditorDrawMode_None: {

            if (IsKeyPressed(Key_P)) {

                printf("LineSegment segments[] = {\n");
                for (const LineSegment& segment : editorState.vsegments) {
                    printf("\t{ { %d, %d }, { %d, %d }, %d, %d, %d },\n",
                        (s32)segment.v1.x / 32,
                        (s32)segment.v1.y / 32,
                        (s32)segment.v2.x / 32,
                        (s32)segment.v2.y / 32,
                        segment.frontSector,
                        segment.backSector);
                }
                printf("};\n");

                printf("Edge edges[] = {\n");
                s32 segIndex = 0;
                for (const auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
                    for (const auto& edge : edges) {
                        const LineSegment& segment = editorState.vsegments[edge.seg];
                        printf("\t{ %d, %s },\n", edge.seg, edge.reversed ? "true" : "false");
                    }
                }
                printf("};\n");

                printf("Sector sectors[] = {\n");
                s32 edgeIndex = 0;
                for (const auto& [sid, edges] : editorState.sectorEdgeMap) {
                    s32 edgeCount = (s32)edges.size();
                    printf("\t{ %d, %d, %d, %d },\n", edgeIndex, edgeCount, 0, 128 / 32);
                    edgeIndex += edgeCount;
                }
                printf("};\n");
            }

            // Handle moving vertices
            if (IsMouseButtonPressed(MouseButton_Left)) {
                const f32 threshold = 32.0f / editorState.camera.pixelsPerUnit;
                glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());

                for (s32 i = 0; i < editorState.vertexCount; ++i) {
                    MapVertex* mapVertex = editorState.vertices[i];
                    glm::vec2 vertexPos = glm::vec2(mapVertex->x, mapVertex->y);
                    if (glm::distance(worldMousePos, vertexPos) < threshold) {
                        editorState.activeVertex = mapVertex;
                        break;
                    }
                }
            }

            if (editorState.activeVertex != nullptr) {
                if (IsMouseButtonDown(MouseButton_Left)) {
                    glm::vec2 mousePos = GetMousePosition();
                    glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, mousePos);
                    editorState.activeVertex->x = worldMousePos.x;
                    editorState.activeVertex->y = worldMousePos.y;
                }

                if (IsMouseButtonReleased(MouseButton_Left)) {
                    glm::ivec2 snapPos = GetSnappedMousePosition(&editorState);
                    editorState.activeVertex->x = snapPos.x;
                    editorState.activeVertex->y = snapPos.y;
                    editorState.activeVertex = nullptr;
                }
            }

            // Handle adding vertices
            if (IsKeyPressed(Key_Space)) {
                glm::ivec2 mousePos = GetSnappedMousePosition(&editorState);
                AddVertex(&editorState, mousePos);

                // Check if we have completed a loop
                if (editorState.tVertexCount >= 3) {
                    glm::ivec2 firstPoint = editorState.tVertices[0];
                    glm::ivec2 lastPoint = editorState.tVertices[editorState.tVertexCount - 1];

                    if (firstPoint == lastPoint) {
                        // Remove the last point (duplicate of first)
                        editorState.tVertexCount--;
                        CompleteSector(&editorState);
                        editorState.tVertexCount = 0;
                    }
                }
            }

            // Draw active sector walls
            if (editorState.tVertexCount > 0) {
                for (s32 i = 0; i < editorState.tVertexCount - 1; i++) {
                    glm::vec2 v1 = glm::vec2(editorState.tVertices[i + 0]);
                    glm::vec2 v2 = glm::vec2(editorState.tVertices[i + 1]);
                    DrawWall(&editorState.camera, v1, v2, COLOR_WHITE);
                }

                glm::vec2 mousePos = GetMousePosition();
                glm::vec2 v1 = glm::vec2(editorState.tVertices[editorState.tVertexCount - 1]);
                glm::vec2 v2 = Camera2D_ScreenToWorld(&editorState.camera, mousePos);
                DrawWall(&editorState.camera, v1, v2, COLOR_WHITE);
            }

            // Draw map walls
            for (u32 i = 0; i < editorState.wallCount; ++i) {
                MapWall* wall = editorState.walls[i];
                glm::vec2 v1 = glm::vec2(wall->v1->x, wall->v1->y);
                glm::vec2 v2 = glm::vec2(wall->v2->x, wall->v2->y);

                if (wall->twin != nullptr) {
                    DrawWall(&editorState.camera, v1, v2, COLOR_RED);
                } else {
                    DrawWall(&editorState.camera, v1, v2, COLOR_WHITE);
                }
            }


            // Find closest point on any wall
            editorState.closestWall = nullptr;
            const f32 closestWallThreshold = 28.0f;
            for (u32 i = 0; i < editorState.wallCount; ++i) {
                MapWall* wall = editorState.walls[i];
                glm::vec2 v1 = glm::vec2(wall->v1->x, wall->v1->y);
                glm::vec2 v2 = glm::vec2(wall->v2->x, wall->v2->y);
                glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());
                glm::vec2 closestPoint = ClosestPointOnSegment(worldMousePos, v1, v2);
                if (glm::distance(closestPoint, worldMousePos) < closestWallThreshold) {
                    DrawClosestPointVertex(&editorState.camera, closestPoint);
                    editorState.closestWall = wall;
                    editorState.closestPointOnWall = closestPoint;
                }
            }

            if (editorState.closestWall && IsKeyPressed(Key_I)) {
                SplitWall(&editorState, editorState.closestWall, editorState.closestPointOnWall);
            }
            

            // Iterate through all sectors and draw a dummy vertex on the sectors edge
            glm::vec2 closestPoint;
            Edge* closestEdge = nullptr;
            s32 closestPointSector = -1;
            f32 closestDistanceThreshold = 28.0f;
            for (auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
                for (Edge& edge : edges) {
                    LineSegment* segment = &editorState.vsegments[edge.seg];
                    glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());
                    closestPoint = ClosestPointOnSegment(worldMousePos, segment->v1, segment->v2);
                    if (glm::distance(closestPoint, worldMousePos) < closestDistanceThreshold) {
                        closestPointSector = sectorIndex;
                        closestEdge = &edge;
                        goto here;
                    }
                }
            }
        here:
            if (closestPointSector != -1) {
                if (IsKeyPressed(Key_I)) {
                    glm::ivec2 newVertex = SnapToGrid(&editorState, closestPoint);
                    LineSegment& originalSegment = editorState.vsegments[closestEdge->seg];
                    LineSegment newSegment = originalSegment;

                    originalSegment.v2 = glm::vec2(newVertex);
                    newSegment.v1 = glm::vec2(newVertex);

                    editorState.vsegments.push_back(newSegment);

                    Edge newEdge = *closestEdge;
                    newEdge.seg = (s32)editorState.vsegments.size() - 1;

                    auto it = std::find(
                        editorState.sectorEdgeMap[closestPointSector].begin(),
                        editorState.sectorEdgeMap[closestPointSector].end(),
                        *closestEdge
                    );
                    if (it != editorState.sectorEdgeMap[closestPointSector].end()) {
                        editorState.sectorEdgeMap[closestPointSector].insert(it + 1, newEdge);
                    }
                    int wait = 0;
                }

                DrawClosestPointVertex(camera, closestPoint);
            }
        } break;

        case EditorDrawMode_Sector: {

            if (IsKeyPressed(Key_Escape)) {
                editorState.vpoints.clear();
                editorState.mode = EditorDrawMode_None;
            }

            // DrawModeSector_Update(&editorState, dt);
            // DrawModeSector_Render(&editorState);
        } break;

        case EditorDrawMode_MoveVertex: {
            
        } break;
    }

    DrawCursor();
    Renderer2D_EndFrame();
}

void Editor_ChangeState(EditorState* state, EditorDrawMode newMode) {
    state->mode = newMode;
}

glm::ivec2 SnapToGrid(const EditorState* state, glm::vec2 worldPos) {
    s32 gridSize = state->gridSize;
    glm::ivec2 snappedPos = glm::ivec2(
        (s32)((worldPos.x + (f32)(gridSize / 2)) / (f32)gridSize) * gridSize,
        (s32)((worldPos.y + (f32)(gridSize / 2)) / (f32)gridSize) * gridSize
    );
    return snappedPos;
}
glm::ivec2 GetSnappedMousePosition(const EditorState* state) {
    glm::vec2 mousePos = GetMousePosition();
    glm::vec2 worldPos = Camera2D_ScreenToWorld(&state->camera, mousePos);
    glm::vec2 snappedPos = SnapToGrid(state, worldPos);
    return snappedPos;
}