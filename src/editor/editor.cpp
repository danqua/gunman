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
    glm::vec2 mousePos = editorState.mousePosition;
    f32 halfSize = 15.0f;
    Renderer2D_DrawLine(mousePos - glm::vec2(halfSize, 0.0f), mousePos + glm::vec2(halfSize, 0.0f), COLOR_RED);
    Renderer2D_DrawLine(mousePos - glm::vec2(0.0f, halfSize), mousePos + glm::vec2(0.0f, halfSize), COLOR_RED);
}

void Editor_Init() {
    s32 windowWidth = Platform_GetWindowWidth();
    s32 windowHeight = Platform_GetWindowHeight();
    editorState.camera = CreateDefaultCamera2D(windowWidth, windowHeight);
    Platform_SetMouseCaptured(true);
    Renderer2D_SetSize((f32)windowWidth, (f32)windowHeight);

}

void Editor_UpdateAndRender(f32 dt) {
    if (IsMouseButtonDown(MouseButton_Middle)) {
        /*f32 pixelPerWorldUnit = (f32)Platform_GetWindowWidth() / 1280.0f;
        glm::vec2 mousePos = GetMousePosition();
        glm::vec2 worldMousePos = editorState.cameraPosition + editorState.mousePosition;
        editorState.cameraPosition -= worldMousePos;
        Renderer2D_SetViewOffset(editorState.cameraPosition.x, editorState.cameraPosition.y);*/
    }

    // const f32 oldZoomLevel = editorState.zoomLevel;
    // editorState.zoomLevel += GetMouseWheelDelta();
    // if (oldZoomLevel != editorState.zoomLevel) {
    //     editorState.zoomLevel = glm::clamp(editorState.zoomLevel, 1.0f, 5.0f);
    //     f32 windowWidth = (f32)Platform_GetWindowWidth();
    //     f32 windowHeight = (f32)Platform_GetWindowHeight();
    //     Renderer2D_SetSize(windowWidth / editorState.zoomLevel, windowHeight / editorState.zoomLevel);
    // 
    //     // TODO: Adjust camera position to zoom towards mouse position
    // }

    editorState.mousePosition = GetMousePosition();

    // Screen to world coordinates
    glm::vec2 worldMousePos = Camera2D_ScreenToWorld(&editorState.camera, editorState.mousePosition);
    editorState.snappedPos = glm::ivec2(
        (s32)((worldMousePos.x + 16.0f) / 32.0f) * 32,
        (s32)((worldMousePos.y + 16.0f) / 32.0f) * 32
    );

    // Draw grid
    Camera camera = Camera_CreateOrthographic(0.0f, (f32)Platform_GetWindowWidth(), (f32)Platform_GetWindowHeight(), 0.0f, -1.0f, 1.0f);
    glm::mat4 projection = Camera_GetProjectionMatrix(&camera);
    glm::mat4 view = Camera2D_GetViewMatrix(&editorState.camera);
    Renderer2D_BeginFrame(&projection, &view);

    s32 windowWidth = Platform_GetWindowWidth();
    s32 windowHeight = Platform_GetWindowHeight();

    for (s32 y = 0; y <= windowHeight; y += 32) {
        Renderer2D_DrawLine(glm::vec2(0.0f, (f32)y), glm::vec2((f32)windowWidth, (f32)y), COLOR_DARK_GRAY);
    }

    for (s32 x = 0; x <= windowWidth; x += 32) {
        Renderer2D_DrawLine(glm::vec2((f32)x, 0.0f), glm::vec2((f32)x, (f32)windowHeight), COLOR_DARK_GRAY);
    }

    for (const auto& [sectorIndex, edges] : editorState.sectorEdgeMap) {
        for (const auto& edge : edges) {
            const LineSegment& seg = editorState.segments[edge.seg];
            Renderer2D_DrawLine(seg.v1, seg.v2, seg.backSector != -1 ? COLOR_RED : COLOR_WHITE);
            Renderer2D_DrawRect(seg.v1 - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
            Renderer2D_DrawRect(seg.v2 - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
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

glm::ivec2 Editor_SnapToGrid(const EditorState* state, const glm::vec2& position) {
    glm::vec2 worldPos = Camera2D_ScreenToWorld(&state->camera, position);
    glm::ivec2 snappedPos = glm::ivec2(
        (s32)((worldPos.x + 16.0f) / 32.0f) * 32,
        (s32)((worldPos.y + 16.0f) / 32.0f) * 32
    );
    return snappedPos;
}

void Editor_ChangeState(EditorState* state, EditorDrawMode newMode) {
    state->mode = newMode;
}
