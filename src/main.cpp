#include "core/platform.h"
#include "core/audio.h"
#include "core/input.h"
#include "core/math.h"
#include "core/memory.h"
#include "core/containers.h"
#include "renderer/rhi.h"
#include "renderer/camera.h"
#include "renderer/image.h"
#include "renderer/renderer_2d.h"
#include "renderer/renderer.h"
#include "renderer/sprite.h"

#include "scene/camera_controller.h"
#include "scene/transform.h"

struct AABB {
    glm::vec2 min;
    glm::vec2 max;
};

bool IntersectRayAABB(glm::vec2 rayOrigin, glm::vec2 rayDir, const AABB& aabb, f32* tMinOut, f32* tMaxOut) {
    f32 tMin = (aabb.min.x - rayOrigin.x) / rayDir.x;
    f32 tMax = (aabb.max.x - rayOrigin.x) / rayDir.x;

    if (tMin > tMax) {
        f32 tmp = tMin;
        tMin = tMax;
        tMax = tmp;
    }

    f32 tyMin = (aabb.min.y - rayOrigin.y) / rayDir.y;
    f32 tyMax = (aabb.max.y - rayOrigin.y) / rayDir.y;

    if (tyMin > tyMax) {
        f32 tmp = tyMin;
        tyMin = tyMax;
        tyMax = tmp;
    }

    if ((tMin > tyMax) || (tyMin > tMax)) {
        return false;
    }

    if (tyMin > tMin) {
        tMin = tyMin;
    }

    if (tyMax < tMax) {
        tMax = tyMax;
    }

    if (tMax < 0) {
        return false;
    }

    *tMinOut = tMin;
    *tMaxOut = tMax;

    return true;
}

glm::vec2 ProjectPointOnLine(glm::vec2 point, glm::vec2 lineStart, glm::vec2 lineEnd) {
    glm::vec2 lineDir = lineEnd - lineStart;
    glm::vec2 pointDir = point - lineStart;
    f32 t = glm::dot(pointDir, lineDir) / glm::dot(lineDir, lineDir);
    t = glm::clamp(t, 0.0f, 1.0f);
    return lineStart + t * lineDir;
}

struct LineSegment {
    glm::vec2 v1;
    glm::vec2 v2;
    s32 frontSector;
    s32 backSector;
    u32 flags;
};

struct Edge {
    s32 seg;
    b32 reversed;
};

struct Sector {
    s32 firstEdge;
    s32 edgeCount;
    f32 floorHeight;
    f32 ceilingHeight;
};

glm::vec2 ClosestPointOnSegment(glm::vec2 point, glm::vec2 v1, glm::vec2 v2) {
    glm::vec2 result = ProjectPointOnLine(point, v1, v2);
    return result;
}

LineSegment segments[] = {
    { {  1, 1 }, {  4, 1 }, 0, -1, 0 },
    { {  4, 1 }, {  6, 3 }, 0, -1, 0 },
    { {  6, 3 }, {  6, 5 }, 0,  1, 0 },
    { {  6, 5 }, {  1, 5 }, 0, -1, 0 },
    { {  1, 5 }, {  1, 1 }, 0, -1, 0 },
    { {  6, 3 }, {  8, 3 }, 1, -1, 0 },
    { {  8, 3 }, {  8, 5 }, 1,  2, 0 },
    { {  8, 5 }, {  6, 5 }, 1, -1, 0 },
    { {  8, 3 }, {  8, 1 }, 2, -1, 0 },
    { {  8, 1 }, { 11, 1 }, 2, -1, 0 },
    { { 11, 1 }, { 11, 3 }, 2,  3, 0 },
    { { 11, 3 }, { 11, 5 }, 2, -1, 0 },
    { { 11, 5 }, {  8, 5 }, 2, -1, 0 },
    { { 11, 1 }, { 13, 1 }, 3, -1, 0 },
    { { 13, 1 }, { 13, 3 }, 3, -1, 0 },
    { { 13, 3 }, { 11, 3 }, 3, -1, 0 }
};

Edge edges[] = {
    {  0, false },
    {  1, false },
    {  2, false },
    {  3, false },
    {  4, false },
    {  2, true  },
    {  5, false },
    {  6, false },
    {  7, false },
    {  8, false },
    {  9, false },
    { 10, false },
    { 11, false },
    { 12, false },
    {  6, true  },
    { 13, false },
    { 14, false },
    { 15, false },
    { 10, true  },
};

Sector sectors[] = {
    {  0, 5, 0, 4 },
    {  5, 4, 0, 4 },
    {  9, 6, 0, 4 },
    { 15, 4, 0, 4 }
};

struct Player {
    glm::vec2 position;
    f32 angle;
    f32 radius;
    s32 currentSector;
};

void DrawSector(const Sector* sector, Color color) {
    for (s32 i = 0; i < sector->edgeCount; ++i) {
        const Edge* edge = &edges[sector->firstEdge + i];
        const LineSegment* segment = &segments[edge->seg];

        if (segment->backSector != -1 || edge->reversed) {
            Renderer2D_DrawLine(segment->v1, segment->v2, COLOR_DARK_GRAY);
        } else {
            Renderer2D_DrawLine(segment->v1, segment->v2, color);
        }
    }
}

void UpdatePlayer(Player* player, f32 dt) {
    f32 movementSpeed = 3.0f;
    f32 rotationSpeed = 1.0f;
    glm::vec2 forward = glm::vec2(glm::cos(player->angle), glm::sin(player->angle));
    if (IsKeyDown(Key_Left)) {
        player->angle -= glm::pi<float>() * rotationSpeed * dt;
    }
    if (IsKeyDown(Key_Right)) {
        player->angle += glm::pi<float>() * rotationSpeed * dt;
    }
    if (IsKeyDown(Key_Up)) {
        player->position += forward * movementSpeed * dt;
    }
    if (IsKeyDown(Key_Down)) {
        player->position -= forward * movementSpeed * dt;
    }

    const Sector* sector = &sectors[player->currentSector];
    for (s32 i = 0; i < sector->edgeCount; ++i) {
        const Edge* edge = &edges[sector->firstEdge + i];
        const LineSegment* segment = &segments[edge->seg];
        glm::vec2 closestPoint = ClosestPointOnSegment(player->position, segment->v1, segment->v2);
        if (glm::distance(closestPoint, player->position) <= player->radius) {
            if (segment->backSector != -1) {
                f32 dot = glm::dot(closestPoint, closestPoint - player->position);

                if (edge->reversed && segment->frontSector != player->currentSector) {
                    if (dot > 0 && player->currentSector != segment->frontSector) {
                        player->currentSector = segment->frontSector;
                        break;
                    }
                } else {
                    if (dot < 0 && player->currentSector != segment->backSector) {
                        player->currentSector = segment->backSector;
                        break;
                    }
                }
                continue;
            }


            glm::vec2 edgeDir = segment->v2 - segment->v1;
            glm::vec2 edgeNormal = glm::normalize(glm::vec2(-edgeDir.y, edgeDir.x));
            glm::vec2 penetrationVector = edgeNormal * (player->radius - glm::distance(closestPoint, player->position));
            player->position += penetrationVector;
        }
    }
}

void DrawPlayer(const Player* player) {
    glm::vec2 dir = glm::vec2(glm::cos(player->angle), glm::sin(player->angle));
    Renderer2D_DrawLine(player->position, player->position + dir * player->radius, COLOR_YELLOW);
    Renderer2D_DrawRect(player->position - glm::vec2(player->radius), glm::vec2(player->radius * 2), COLOR_YELLOW);
}

int main(int argc, char** argv)
{
    Platform_InitWindow("Gunman", 1920, 1080);
    //Platform_PlayAudioClip(audio, true);

    void* memory = Platform_Alloc(Megabytes(12));
    Platform_Assert(memory, "Failed to allocate memory.");

    Arena permanentStorage = {};
    Arena_Init(&permanentStorage, Megabytes(8), memory);

    Arena transientStorage = {};
    Arena_Init(&transientStorage, Megabytes(4), (u8*)memory + Megabytes(8));
  
    RHI_Init();
    Renderer_Init(&permanentStorage);
    Renderer2D_Init(&permanentStorage);
    Renderer_SetSize(1920, 1080);

    Renderer2D_SetSize(1920.0f / 64.0f, 1080.0f / 64.0f);
    
    Audio_Init(&permanentStorage);

    Player player = {};
    player.position = glm::vec2(3.0f, 3.0f);
    player.angle = 0.0f;
    player.radius = 0.25f;
    player.currentSector = 0;

    AABB box = {};
    box.min = glm::vec2(5.0f, 2.0f);
    box.max = glm::vec2(7.0f, 4.0f);

    while (!Platform_WindowShouldClose())
    {
        Platform_PollEvents();
        Audio_Update();

        if (IsKeyPressed(Key_Escape))
        {
            Platform_CloseWindow();
        }

        static f64 lastTime = Platform_GetTime();
        f64 currentTime = Platform_GetTime();
        f32 deltaTime = (f32)(currentTime - lastTime);
        lastTime = currentTime;


        Renderer2D_BeginFrame();

        for (s32 i = 0; i < 4; ++i) {
            DrawSector(&sectors[i], i == player.currentSector ? COLOR_YELLOW : COLOR_WHITE);
        }

        Color boxColor = COLOR_WHITE;
        f32 tMin, tMax;
        
        glm::vec2 rayOrigin = player.position;
        glm::vec2 rayDir = glm::vec2(glm::cos(player.angle), glm::sin(player.angle));

        if (IntersectRayAABB(rayOrigin, rayDir, box, &tMin, &tMax)) {
            boxColor = COLOR_RED;

            Renderer2D_DrawLine(rayOrigin, rayOrigin + rayDir * tMin, COLOR_GREEN);
        }
        Renderer2D_DrawRect(box.min, box.max - box.min, boxColor);

        UpdatePlayer(&player, deltaTime);
        DrawPlayer(&player);

        Renderer2D_EndFrame();
        Platform_SwapBuffers();
        Input_NextFrame();
    }

    RHI_Shutdown();
    Audio_Shutdown();
    return 0;
}