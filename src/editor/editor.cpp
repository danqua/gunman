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

void Editor_Init() {
    editorState.gridSize = 32;
    s32 windowWidth = Platform_GetWindowWidth();
    s32 windowHeight = Platform_GetWindowHeight();
    editorState.camera = CreateDefaultCamera2D(windowWidth, windowHeight);
    
    Platform_SetMouseCaptured(true);
    Renderer2D_SetSize((f32)windowWidth, (f32)windowHeight);

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
            const LineSegment& seg = editorState.segments[edge.seg];
            Renderer2D_DrawLine(seg.v1, seg.v2, seg.backSector != -1 ? COLOR_RED : COLOR_WHITE);
            DrawVertex(&editorState.camera, seg.v1);
            DrawVertex(&editorState.camera, seg.v2);
        }
    }

    switch (editorState.mode) {
        case EditorDrawMode_None: {

            if (IsKeyPressed(Key_P)) {

                printf("LineSegment segments[] = {\n");
                for (const LineSegment& segment : editorState.segments) {
                    printf("\t{ { %d, %d }, { %d, %d }, %d, %d, %d },\n",
                        (s32)segment.v1.x / 32,
                        (s32)segment.v1.y / 32,
                        (s32)segment.v2.x / 32,
                        (s32)segment.v2.y / 32,
                        segment.frontSector,
                        segment.backSector,
                        segment.flags);
                }
                printf("};\n");

                printf("Edge edges[] = {\n");
                s32 segIndex = 0;
                for (const auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
                    for (const auto& edge : edges) {
                        const LineSegment& segment = editorState.segments[edge.seg];
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



            if (IsKeyPressed(Key_Space)) {
                editorState.mode = EditorDrawMode_Sector;
                DrawModeSector_Enter(&editorState);
            }

            // Iterate through all sectors and draw a dummy vertex on the sectors edge
            glm::vec2 closestPoint;
            s32 closestPointSector = -1;
            f32 closestDistanceThreshold = 28.0f;
            for (const auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
                for (const auto& edge : edges) {
                    LineSegment* segment = &editorState.segments[edge.seg];
                    glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, GetMousePosition());
                    closestPoint = ClosestPointOnSegment(worldMousePos, segment->v1, segment->v2);
                    if (glm::distance(closestPoint, worldMousePos) < closestDistanceThreshold) {
                        closestPointSector = sectorIndex;
                        goto here;
                    }
                }
            }
            here:
            if (closestPointSector != -1) {
                f32 halfLength = 5.0f;
                Renderer2D_DrawLine(closestPoint + glm::vec2(-halfLength, -halfLength), closestPoint + glm::vec2(halfLength, halfLength), COLOR_ORANGE);
                Renderer2D_DrawLine(closestPoint + glm::vec2(-halfLength, halfLength), closestPoint + glm::vec2(halfLength, -halfLength), COLOR_ORANGE);
            }
            /*
            glm::vec2 closestPoint;
            bool foundClosestPoint = false;
            f32 closestPointDistThreshold = 28.0f;
            u64 sector = -1;
            Edge* closestEdge = nullptr;
            for (const auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
                for (const auto& edge : edges) {
                    LineSegment* segment = &editorState.segments[edge.seg];
                    closestPoint = ClosestPointOnSegment(editorState.mousePosition, segment->v1, segment->v2);
                    f32 dist = glm::distance(closestPoint, editorState.mousePosition);
                    if (dist < closestPointDistThreshold) {
                        foundClosestPoint = true;
                        sector = sectorIndex;
                        closestEdge = (Edge*)&edge;
                        break;
                    } else {
                        int wait = 0;
                    }
                }
                if (foundClosestPoint) break;
            }

            if (foundClosestPoint) {
                if (IsKeyPressed(Key_V)) {
                    glm::ivec2 point = Editor_SnapToGrid(&editorState, closestPoint);
                    LineSegment& segment = editorState.segments[closestEdge->seg];
                    if (PointOnSegment(glm::vec2(point), segment.v1, segment.v2)) {
                        printf("DONT ADD A VERTEX!\n");
                    }
                }
                if (IsKeyPressed(Key_I)) {
                    glm::ivec2 newVertex = Editor_SnapToGrid(&editorState, closestPoint);
                    LineSegment& originalSegment = editorState.segments[closestEdge->seg];
                    LineSegment newSegment = originalSegment;

                    originalSegment.v2 = glm::vec2(newVertex);
                    newSegment.v1 = glm::vec2(newVertex);

                    editorState.segments.push_back(newSegment);

                    Edge newEdge = *closestEdge;
                    newEdge.seg = (s32)editorState.segments.size() - 1;

                    auto it = std::find(
                        editorState.sectorEdgeMap[sector].begin(),
                        editorState.sectorEdgeMap[sector].end(),
                        *closestEdge
                    );
                    if (it != editorState.sectorEdgeMap[sector].end()) {
                        editorState.sectorEdgeMap[sector].insert(it + 1, newEdge);
                    }
                    int wait= 0;
                }
                


                f32 halfLength = 5.0f;
                Renderer2D_DrawLine(closestPoint + glm::vec2(-halfLength,-halfLength), closestPoint + glm::vec2(halfLength,  halfLength), COLOR_ORANGE);
                Renderer2D_DrawLine(closestPoint + glm::vec2(-halfLength, halfLength), closestPoint + glm::vec2(halfLength, -halfLength), COLOR_ORANGE);
            }
            */
        } break;

        case EditorDrawMode_Sector: {

            if (IsKeyPressed(Key_Escape)) {
                editorState.points.clear();
                editorState.mode = EditorDrawMode_None;
            }

            DrawModeSector_Update(&editorState, dt);
            DrawModeSector_Render(&editorState);
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