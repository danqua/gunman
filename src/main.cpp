#include "core/platform.h"
#include "core/assets.h"
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

template <typename T> int Sign(T val) {
    return (T(0) < val) - (val < T(0));
}

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

bool IntersectRayLineSegment(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 v1, glm::vec2 v2, f32* tOut) {
    glm::vec2 segDir = v2 - v1;
    glm::vec2 diff = v1 - rayOrigin;

    float denom = rayDir.x * segDir.y - rayDir.y * segDir.x;

    const f32 epsilon = 1e-6f;
    if (fabsf(denom) < epsilon) {// Parallel
        return false;
    }

    float t = (diff.x * segDir.y - diff.y * segDir.x) / denom;
    float u = (diff.x * rayDir.y - diff.y * rayDir.x) / denom;

    if (t >= 0 && u >= 0 && u <= 1) {
        *tOut = t;
        return true;
    }

    return false;
}

bool IntersectRayCircle(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 center, f32 radius, f32* tOut) {
    glm::vec2 circleDir = rayOrigin - center;

    f32 b = glm::dot(circleDir, rayDir);
    f32 c = glm::dot(circleDir, circleDir) - radius * radius;

    if (c > 0.0f && b > 0.0f) {
        return false;
    }

    f32 discr = b * b - c;
    if (discr < 0.0f) {
        return false;
    }

    f32 t = -b - glm::sqrt(discr);
    *tOut = glm::max(t, 0.0f);
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
    bool reversed;
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
    { {  1, 1 }, {  4, 1 }, 0, -1, 0 }, // 0
    { {  4, 1 }, {  6, 3 }, 0, -1, 0 }, // 1
    { {  6, 3 }, {  6, 5 }, 0,  1, 0 }, // 2
    { {  6, 5 }, {  1, 5 }, 0, -1, 0 }, // 3
    { {  1, 5 }, {  1, 1 }, 0, -1, 0 }, // 4
    { {  6, 3 }, {  8, 3 }, 1, -1, 0 }, // 5
    { {  8, 3 }, {  8, 5 }, 1,  2, 0 }, // 6
    { {  8, 5 }, {  6, 5 }, 1, -1, 0 }, // 7
    { {  8, 3 }, {  8, 1 }, 2, -1, 0 }, // 8
    { {  8, 1 }, { 11, 1 }, 2, -1, 0 }, // 9
    { { 11, 1 }, { 11, 3 }, 2,  3, 0 }, // 10
    { { 11, 3 }, { 11, 5 }, 2, -1, 0 }, // 11
    { { 11, 5 }, {  8, 5 }, 2, -1, 0 }, // 12
    { { 11, 1 }, { 13, 1 }, 3, -1, 0 }, // 13
    { { 13, 1 }, { 13, 3 }, 3,  4, 0 }, // 14
    { { 13, 3 }, { 11, 3 }, 3, -1, 0 }, // 15

    // sec[4]
    { { 13, 1 }, { 14, 1 }, 4, -1, 0 }, // 16
    { { 14, 1 }, { 14, 4 }, 4,  5, 0 }, // 17
    { { 14, 4 }, { 14, 6 }, 4,  6, 0 }, // 18
    { { 14, 6 }, { 14, 7 }, 4,  8, 0 }, // 19
    { { 14, 7 }, { 13, 7 }, 4, -1, 0 }, // 20
    { { 13, 7 }, { 13, 3 }, 4, -1, 0 }, // 21

    // sec[5]
    { { 14, 1 }, { 18, 1 }, 5, -1, 0 }, // 22
    { { 18, 1 }, { 18, 4 }, 5, -1, 0 }, // 23
    { { 18, 4 }, { 17, 4 }, 5,  7, 0 }, // 24
    { { 17, 4 }, { 14, 4 }, 5,  6, 0 }, // 25
    // 14, 1 <- 14, 4

    // sec[6]
    { { 17, 4 }, { 17, 6 },  6,  7, 0 }, // 26
    { { 17, 6 }, { 14, 6 },  6,  8, 0 }, // 27
    // 14, 4 <- 14, 6
    
    // sec[7]
    { { 18, 4 }, { 18, 7 }, 7, -1, 0 }, // 28
    { { 18, 7 }, { 17, 7 }, 7, -1, 0 }, // 29
    { { 17, 7 }, { 17, 6 }, 7,  8, 0 }, // 30
    // 17, 4 <- 17, 6

    // sec[8]
    { { 17, 7 }, { 14, 7 }, 8, -1, 0 }, // 31
    // 14, 6 <- 14, 7
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

    { 16, false },
    { 17, false },
    { 18, false },
    { 19, false },
    { 20, false },
    { 21, false },
    { 14, true  },

    { 22, false },
    { 23, false },
    { 24, false },
    { 25, false },
    { 17, true  },

    { 25, true  },
    { 26, false },
    { 27, false },
    { 18, true  },

    { 28, false },
    { 29, false },
    { 30, false },
    { 26, true  },
    { 24, true  },

    { 27, true  },
    { 30, true  },
    { 31, false },
    { 19, true  }
};

Sector sectors[] = {
    {  0, 5,  0, 3 }, // 0
    {  5, 4,  1, 4 }, // 1
    {  9, 6,  0, 3 }, // 2
    { 15, 4, -1, 2 }, // 3
    { 19, 7,  0, 4 }, // 4
    { 26, 5,  0, 4 }, // 5
    { 31, 4, 0.25f, 4 }, // 6
    { 35, 5,  0, 4 }, // 7
    { 40, 4 , 0, 4 }  // 8
};

const u32 sectorCount = 9;

struct Player {
    glm::vec2 position;
    f32 angle;
    f32 radius;
    s32 currentSector;
};

struct Enemy {
    glm::vec2 position;
    f32 angle;
    f32 radius;
};

bool IsPortal(LineSegment* segment) {
    return (segment->backSector != -1);
}

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

static inline f32 SignedDistanceToLine(glm::vec2 p, glm::vec2 a, glm::vec2 b) {
    glm::vec2 e = b - a;
    return (e.x * (p.y - a.y) - e.y * (p.x - a.x));
}

void UpdatePlayer(Player* player, f32 dt) {
    static glm::vec2 lastPosition;
    lastPosition = player->position;

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

        f32 sd1 = SignedDistanceToLine(lastPosition, segment->v1, segment->v2);
        f32 sd2 = SignedDistanceToLine(player->position, segment->v1, segment->v2);
        if (segment->backSector != -1 && Sign(sd1) != Sign(sd2)) {
            s32 lastSector = player->currentSector;
            player->currentSector = edge->reversed ? segment->frontSector : segment->backSector;
            printf("%d -> %d\n", lastSector, player->currentSector);
            break;
        }

        glm::vec2 closestPoint = ClosestPointOnSegment(player->position, segment->v1, segment->v2);
        if (glm::distance(closestPoint, player->position) <= player->radius) {
            if (segment->backSector != -1) {
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

void CreateWall(glm::vec2 v1, glm::vec2 v2, f32 floorHeight, f32 ceilHeight, DynamicArray<Vertex>& vertices, DynamicArray<u32>& indices) {

    glm::vec3 verts[4] = {
        glm::vec3(v1.x, floorHeight, v1.y),
        glm::vec3(v2.x, floorHeight, v2.y),
        glm::vec3(v2.x, ceilHeight, v2.y),
        glm::vec3(v1.x, ceilHeight, v1.y)
    };
    glm::vec3 normal = glm::normalize(glm::vec3(v2.y - v1.y, 0.0f, v1.x - v2.x));

    u32 baseIndex = vertices.size;

    for (s32 i = 0; i < 4; ++i) {
        Vertex vert;
        vert.position = verts[i];
        vert.normal = normal;
        vertices.Add(vert);
    }

    indices.Add(baseIndex + 0);
    indices.Add(baseIndex + 1);
    indices.Add(baseIndex + 2);
    indices.Add(baseIndex + 2);
    indices.Add(baseIndex + 3);
    indices.Add(baseIndex + 0);
}

void CreateFloorAndCeiling(const Sector* sector, DynamicArray<Vertex>& vertices, DynamicArray<u32>& indices) {

}

Mesh CreateSectorMesh(const Sector* sector) {
    DynamicArray<Vertex> vertices;
    DynamicArray<u32> indices;


    for (s32 i = 0; i < sector->edgeCount; ++i) {
        const Edge* edge = &edges[sector->firstEdge + i];
        const LineSegment* segment = &segments[edge->seg];

        glm::vec2 segDir = segment->v2 - segment->v1;
        segDir = glm::normalize(glm::vec2(-segDir.y, segDir.x));
        
        // Create portal walls for adjacend sectors with different floor/ceiling heights
        if (segment->backSector != -1) {
            u32 sectorIndex = edge->reversed ? segment->frontSector : segment->backSector;
            const Sector* adjacentSector = &sectors[sectorIndex];

            if (adjacentSector->floorHeight > sector->floorHeight) {
                if (edge->reversed) {
                    CreateWall(segment->v2, segment->v1, sector->floorHeight, adjacentSector->floorHeight, vertices, indices);
                } else {
                    CreateWall(segment->v1, segment->v2, sector->floorHeight, adjacentSector->floorHeight, vertices, indices);
                }
            }

            if (adjacentSector->ceilingHeight < sector->ceilingHeight) {
                if (edge->reversed) {
                    CreateWall(segment->v1, segment->v2, sector->ceilingHeight, adjacentSector->ceilingHeight, vertices, indices);
                } else {
                    CreateWall(segment->v2, segment->v1, sector->ceilingHeight, adjacentSector->ceilingHeight, vertices, indices);
                }
            }            
            continue;
        }

        CreateWall(segment->v1, segment->v2, sector->floorHeight, sector->ceilingHeight, vertices, indices);
    }

    
    for (s32 i = 0; i < sector->edgeCount; ++i) {
        const Edge* edge = &edges[sector->firstEdge + i];
        const LineSegment* segment = &segments[edge->seg];

        if (segment->backSector != -1) {
            continue;
        }

        
    }

    // Create ceiling vertices
    s32 baseIndex = vertices.size;
    for (s32 i = 0; i < sector->edgeCount; ++i) {
        const Edge* edge = &edges[sector->firstEdge + i];
        const LineSegment* segment = &segments[edge->seg];

        Vertex vertex = {};
        if (edge->reversed) {
            vertex.position = glm::vec3(segment->v2.x, sector->ceilingHeight, segment->v2.y);
        } else {
            vertex.position = glm::vec3(segment->v1.x, sector->ceilingHeight, segment->v1.y);
        }
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices.Add(vertex);
    }

    for (s32 i = 0; i < sector->edgeCount - 2; ++i) {
        indices.Add(baseIndex + 0);
        indices.Add(baseIndex + i + 1);
        indices.Add(baseIndex + i + 2);
    }

    // Create floor vertices
    baseIndex = vertices.size;
    for (s32 i = sector->edgeCount - 1; i >= 0; --i) {
        const Edge* edge = &edges[sector->firstEdge + i];
        const LineSegment* segment = &segments[edge->seg];

        Vertex vertex = {};
        if (edge->reversed) {
            vertex.position = glm::vec3(segment->v2.x, sector->floorHeight, segment->v2.y);
        }
        else {
            vertex.position = glm::vec3(segment->v1.x, sector->floorHeight, segment->v1.y);
        }
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices.Add(vertex);
    }

    for (s32 i = 0; i < sector->edgeCount - 2; ++i) {
        indices.Add(baseIndex + 0);
        indices.Add(baseIndex + i + 1);
        indices.Add(baseIndex + i + 2);
    }

    Mesh mesh = CreateMesh(vertices.data, vertices.size, indices.data, indices.size);
    return mesh;
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

    Camera camera = Camera_CreatePerspective(glm::radians(70.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
    CameraController controls = CreateDefaultCameraController();
    Transform cameraTransform = CreateDefaultTransform();

    Renderer2D_SetSize(1920.0f / 64.0f, 1080.0f / 64.0f);
    
    Audio_Init(&permanentStorage);

    Player player = {};
    player.position = glm::vec2(3.0f, 3.0f);
    player.angle = 0.0f;
    player.radius = 0.25f;
    player.currentSector = 0;

    AABB box = {};
    box.min = glm::vec2(4.0f, 2.0f);
    box.max = glm::vec2(5.0f, 3.0f);

    Mesh sectorMeshes[sectorCount];
    for (s32 i = 0; i < sectorCount; ++i) {
        sectorMeshes[i] = CreateSectorMesh(&sectors[i]);
    }

    Material sectorMaterial = {};
    sectorMaterial.shader = RHI_CreateShader(R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aNormal;

        out vec3 vNormal;

        uniform mat4 uProjectionMatrix;
        uniform mat4 uViewMatrix;

        void main() {
            gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPosition, 1.0);
            vNormal = abs(aNormal);
        }
    )", R"(
        #version 330 core
        in vec3 vNormal;
        out vec4 fragColor;

        void main() {
            fragColor = vec4(vNormal, 1.0);
        }
    )");
    sectorMaterial.backfaceCulling = true;

    bool toggleTo3D = false;
    bool debugDraw = false;

    while (!Platform_WindowShouldClose())
    {
        Platform_PollEvents();
        Audio_Update();

        Arena_Clear(&transientStorage);

        if (IsKeyPressed(Key_Escape))
        {
            Platform_CloseWindow();
        }

        if (IsKeyPressed(Key_F1)) {
            toggleTo3D = !toggleTo3D;
            Platform_SetMouseCaptured(toggleTo3D);
        }

        if (IsKeyPressed(Key_F2)) {
            debugDraw = !debugDraw;
        }

        static f64 lastTime = Platform_GetTime();
        f64 currentTime = Platform_GetTime();
        f32 deltaTime = (f32)(currentTime - lastTime);
        lastTime = currentTime;

        UpdateCameraControls(&controls, &cameraTransform, deltaTime);


        if (toggleTo3D) {
            glm::mat4 projection = Camera_GetProjectionMatrix(&camera);
            glm::mat4 view = Transform_GetMatrixInv(&cameraTransform);
            Renderer_BeginFrame(projection, view);

            for (s32 i = 0; i < sectorCount; ++i) {
                Mesh* mesh = &sectorMeshes[i];
                Renderer_DrawMesh(mesh, &sectorMaterial, glm::mat4(1.0f));

                if (debugDraw) {
                    Renderer_DrawBox(mesh->aabb.min, mesh->aabb.max, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                }
            }

            Renderer_EndFrame();

        } else {
            Renderer2D_BeginFrame();

            for (s32 i = 0; i < sectorCount; ++i) {
                DrawSector(&sectors[i], i == player.currentSector ? COLOR_YELLOW : COLOR_WHITE);
            }


            glm::vec2 rayOrigin = player.position;
            glm::vec2 rayDir = glm::vec2(glm::cos(player.angle), glm::sin(player.angle));


            f32* tList = ArenaPushArray(&transientStorage, f32, 256);
            s32 tCount = 0;
            for (s32 i = 0; i < sectorCount; ++i) {
                Sector* sector = &sectors[i];

                for (s32 j = 0; j < sector->edgeCount; ++j) {
                    Edge* edge = &edges[sector->firstEdge + j];
                    LineSegment* segment = &segments[edge->seg];

                    if (IsPortal(segment)) {
                        continue;
                    }

                    f32 t = 0.0f;
                    if (IntersectRayLineSegment(rayOrigin, rayDir, segment->v1, segment->v2, &t)) {
                        tList[tCount++] = t;
                    }
                }
            }


            f32 tMin, tMax;
            if (IntersectRayAABB(rayOrigin, rayDir, box, &tMin, &tMax)) {
                tList[tCount++] = tMin;
            }
            Renderer2D_DrawRect(box.min, box.max - box.min, COLOR_WHITE);

            if (tCount > 0) {
                f32 closestT = FLT_MAX;
                for (s32 i = 0; i < tCount; ++i) {
                    if (tList[i] < closestT) {
                        closestT = tList[i];
                    }
                }

                Renderer2D_DrawLine(rayOrigin, rayOrigin + rayDir * closestT, COLOR_BLUE);
            }


            UpdatePlayer(&player, deltaTime);
            DrawPlayer(&player);

            Renderer2D_EndFrame();
        }
        
        Platform_SwapBuffers();
        Input_NextFrame();
    }

    RHI_Shutdown();
    Audio_Shutdown();
    return 0;
}