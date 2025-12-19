#include "draw_mode_sector.h"

#include "core/input.h"
#include "renderer/renderer_2d.h"
#include "editor.h"

void DrawModeSector_Enter(EditorState* state) {
    glm::ivec2 snappedPos = GetSnappedMousePosition(state);
    state->vpoints.push_back(snappedPos);
}

void DrawModeSector_Exit(EditorState* state) {
    
}

void DrawModeSector_Update(EditorState* state, f32 dt) {

    if (IsKeyPressed(Key_Space)) {
        glm::ivec2 snappedPos = GetSnappedMousePosition(state);

        if (state->vpoints.size() > 0 && snappedPos == state->vpoints[0]) {
            for (u64 i = 0; i < state->vpoints.size(); ++i) {
                
                glm::ivec2 p1 = state->vpoints[i];
                glm::ivec2 p2 = state->vpoints[(i + 1) % state->vpoints.size()];

                u64 segmentIndex = state->vsegments.size();

                // Check if there is already a segment between p1 and p2 (reversed)
                bool segmentExists = false;
                for (const auto& [sectorIndex, edges] : state->sectorEdgeMap) {
                    for (const auto& edge : edges) {
                        LineSegment& seg = state->vsegments[edge.seg];
                        if (seg.v1 == glm::vec2(p2) && seg.v2 == glm::vec2(p1)) {
                            segmentExists = true;

                            Edge newEdge = {};
                            newEdge.seg = edge.seg;
                            newEdge.reversed = true;
                            seg.backSector = state->arrSectorCount;
                            state->sectorEdgeMap[state->arrSectorCount].push_back(newEdge);
                            break;
                        }
                    }
                    if (segmentExists) {
                        break;
                    }
                }

                if (!segmentExists) {
                    LineSegment segment = {};
                    segment.v1 = glm::vec2(p1);
                    segment.v2 = glm::vec2(p2);
                    segment.frontSector = state->arrSectorCount;
                    segment.backSector = -1;
                    state->vsegments.push_back(segment);
                
                    Edge edge = {};
                    edge.seg = segmentIndex;
                    edge.reversed = false;
                
                    state->sectorEdgeMap[state->arrSectorCount].push_back(edge);
                }
            }

            Sector& sector = state->arrsectors[state->arrSectorCount++];
            sector = {};
            //sector.floorHeight = 0.0f;
            //sector.ceilingHeight = 128.0f;

            state->vpoints.clear();

            Editor_ChangeState(state, EditorDrawMode_None);
        } else {
            state->vpoints.push_back(snappedPos);
        }

    }
}

void DrawModeSector_Render(EditorState* state) {
    if (state->vpoints.size() == 0) {
        return;
    }
    glm::ivec2 snappedPos = GetSnappedMousePosition(state);
    glm::vec2 segmentPos = glm::vec2(snappedPos);
    for (s32 i = 0; i < state->vpoints.size() - 1; ++i) {
        glm::vec2 p1 = glm::vec2(state->vpoints[i]);
        glm::vec2 p2 = glm::vec2(state->vpoints[i + 1]);
        Renderer2D_DrawLine(p1, p2, COLOR_WHITE);
        Renderer2D_DrawRect(p1 - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
    }

    glm::vec2 lastPoint = state->vpoints[state->vpoints.size() - 1];
    Renderer2D_DrawLine(lastPoint, segmentPos, COLOR_WHITE);
    Renderer2D_DrawRect(lastPoint - glm::vec2(2.5f), glm::vec2(5.0f, 5.0f), COLOR_WHITE);
}