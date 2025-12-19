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
#include "editor/editor.h"

#include "scene/camera_controller.h"
#include "scene/transform.h"

#include "game/map.h"
#include <mapbox/earcut.hpp>

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

LineSegment segments[] = {
    // Sector
    { { 1, 1 }, { 8, 1 }, 0, -1 }, // 0
    { { 8, 1 }, { 8, 6 }, 0, -1 }, // 1
    { { 8, 6 }, { 6, 6 }, 0,  3 }, // 2
    { { 6, 6 }, { 1, 6 }, 0, -1 }, // 3
    { { 1, 6 }, { 1, 1 }, 0, -1 }, // 4
    
    // Subsector
    { { 6, 2 }, { 7, 2 }, 1, 0 }, // 5
    { { 7, 2 }, { 7, 3 }, 1, 0 }, // 6
    { { 7, 3 }, { 6, 3 }, 1, 0 }, // 7
    { { 6, 3 }, { 6, 2 }, 1, 0 }, // 8

    // Subsector
    { { 2, 4 }, { 3, 4 }, 2, 0 }, // 9
    { { 3, 4 }, { 3, 5 }, 2, 0 }, // 10
    { { 3, 5 }, { 2, 5 }, 2, 0 }, // 11
    { { 2, 5 }, { 2, 4 }, 2, 0 }, // 12

    // Sector
    { { 8,  6 }, { 8, 10 }, 3,  4 }, // 13
    { { 8, 10 }, { 6, 10 }, 3, -1 }, // 14
    { { 6, 10 }, { 6,  6 }, 3, -1 }, // 15
    // seg: 2

    // Sector
    { {  8,  6 }, { 12,  6 }, 4, -1 },  // 16
    { { 12,  6 }, { 12, 10 }, 4, -1 },  // 17
    { { 12, 10 }, {  8, 10 }, 4, -1 },  // 18
    // seg: 13

    // Subsector
    { {  9, 7 }, { 11, 7 }, 5, 4 },  // 19
    { { 11, 7 }, { 11, 9 }, 5, 4 },  // 20
    { { 11, 9 }, {  9, 9 }, 5, 4 },  // 21
    { {  9, 9 }, {  9, 7 }, 5, 4 },  // 22

    // Subsubsector
    { {  9.5, 7.5 }, { 10.5, 7.5 }, 6, 5 }, // 23
    { { 10.5, 7.5 }, { 10.5, 8.5 }, 6, 5 }, // 24
    { { 10.5, 8.5 }, {  9.5, 8.5 }, 6, 5 }, // 25
    { {  9.5, 8.5 }, {  9.5, 7.5 }, 6, 5 }, // 26
};

Edge edges[] = {
    { 0, false },
    { 1, false },
    { 2, false },
    { 3, false },
    { 4, false },

    { 5, false },
    { 6, false },
    { 7, false },
    { 8, false },
    
    {  9, false },
    { 10, false },
    { 11, false },
    { 12, false },

    { 13, false },
    { 14, false },
    { 15, false },
    { 2, true },
    
    { 16, false },
    { 17, false },
    { 18, false },
    { 13, true },
    
    { 19, false },
    { 20, false },
    { 21, false },
    { 22, false },

    { 23, false },
    { 24, false },
    { 25, false },
    { 26, false },
};

static SectorGroup secGroups[] = {
    // Sector
    {  0, 5 },
    {  5, 4 },
    {  9, 4 },

    // Subsector
    { 5, 4 },

    // Subsector
    { 9, 4 },

    // Sector
    { 13, 4 },

    // Sector
    { 17, 4 },
    { 21, 4 },

    // Sector
    { 21, 4 },
    { 25, 4 },

    // Subsector
    { 25, 4 }
};

Sector sectors[] = {
    { 0, 3, 0, 4 },
    { 3, 1, 1, 4 },
    { 4, 1, 2, 4 },
    { 5, 1, -0.2, 3 },
    { 6, 2,  0.4, 4 },
    { 8, 2,  0, 4 },
    { 10, 1, -0.2, 4 },
};
const u32 sectorCount = sizeof(sectors) / sizeof(Sector);

glm::vec2 LineSegment_GetNormal(const LineSegment* segment) {
    glm::vec2 dir = segment->v2 - segment->v1;
    return glm::normalize(glm::vec2(-dir.y, dir.x));
}

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

void DrawSector(const Sector* sector) {
    for (s32 groupIdx = 0; groupIdx < sector->groupCount; ++groupIdx) {
        const SectorGroup* group = &secGroups[sector->firstGroup + groupIdx];
        for (s32 edgeIdx = 0; edgeIdx < group->edgeCount; ++edgeIdx) {
            const Edge* edge = &edges[group->firstEdge + edgeIdx];
            if (edge->reversed) {
                continue;
            }
            
            const LineSegment* seg = &segments[edge->seg];
            glm::vec2 normal = LineSegment_GetNormal(seg);
            glm::vec2 midPoint = (seg->v1 + seg->v2) * 0.5f;
            const f32 normalSize = 0.2f;

            if (seg->backSector != -1) {
                Renderer2D_DrawLine(seg->v1, seg->v2, COLOR_DARK_GRAY);
                Renderer2D_DrawLine(midPoint, midPoint + normal * normalSize, COLOR_DARK_GRAY);
            } else {
                Renderer2D_DrawLine(seg->v1, seg->v2, COLOR_WHITE);
                Renderer2D_DrawLine(midPoint, midPoint + normal * normalSize, COLOR_WHITE);
            }
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
    /*
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
    */
}

void DrawPlayer(const Player* player) {
    glm::vec2 dir = glm::vec2(glm::cos(player->angle), glm::sin(player->angle));
    Renderer2D_DrawLine(player->position, player->position + dir * player->radius, COLOR_YELLOW);
    Renderer2D_DrawRect(player->position - glm::vec2(player->radius), glm::vec2(player->radius * 2), COLOR_YELLOW);
}

void CreateWall(glm::vec2 v1, glm::vec2 v2, f32 floorHeight, f32 ceilHeight, std::vector<Vertex>& vertices, std::vector<u32>& indices) {

    glm::vec3 verts[4] = {
        glm::vec3(v1.x, floorHeight, v1.y),
        glm::vec3(v2.x, floorHeight, v2.y),
        glm::vec3(v2.x, ceilHeight, v2.y),
        glm::vec3(v1.x, ceilHeight, v1.y)
    };

    glm::vec3 vDir = glm::vec3(v2.x - v1.x, 0.0f, v2.y - v1.y);
    glm::vec3 normal = glm::normalize(glm::cross(vDir, glm::vec3(0.0f, 1.0f, 0.0f)));

    u32 baseIndex = vertices.size();

    for (s32 i = 0; i < 4; ++i) {
        Vertex vert;
        vert.position = verts[i];
        vert.normal = normal;

        glm::vec3 uAxis = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 vAxis = glm::cross(uAxis, normal);
        vert.texCoord0.s = glm::dot(vert.position, uAxis) /* + uOffset */;
        vert.texCoord0.t = glm::dot(vert.position, vAxis) /* + vOffset */;

        vertices.push_back(vert);
    }

    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);
}

std::array<Mesh, 2> CreateFloorAndCeiling(const Sector* sector) {
    std::vector<std::array<double, 2>> vertices;
    std::vector<std::vector<std::array<double, 2>>> polygon;

    const SectorGroup* outerSector = &secGroups[sector->firstGroup];
    for (s32 edgeIdx = 0; edgeIdx < outerSector->edgeCount; ++edgeIdx) {
        const Edge* edge = &edges[outerSector->firstEdge + edgeIdx];
        const LineSegment* seg = &segments[edge->seg];
        if (!edge->reversed) {
            vertices.push_back({ seg->v1.x, seg->v1.y });
        }
        else {
            vertices.push_back({ seg->v2.x, seg->v2.y });
        }
    }
    polygon.push_back(vertices);

    for (s32 groupIdx = 1; groupIdx < sector->groupCount; ++groupIdx) {
        const SectorGroup* group = &secGroups[sector->firstGroup + groupIdx];
        std::vector<std::array<double, 2>> holes;
        for (s32 edgeIdx = 0; edgeIdx < group->edgeCount; ++edgeIdx) {
            const Edge* edge = &edges[group->firstEdge + edgeIdx];
            const LineSegment* seg = &segments[edge->seg];
            if (!edge->reversed) {
                holes.push_back({ seg->v1.x, seg->v1.y });
            }
            else {
                holes.push_back({ seg->v2.x, seg->v2.y });
            }
        }
        polygon.push_back(holes);
    }

    std::vector<u32> indices = mapbox::earcut<u32>(polygon);

    std::vector<Vertex> triVertices;
    for (auto pol : polygon) {
        for (auto vertex : pol) {
            Vertex v = {};
            v.position = glm::vec3(vertex[0], sector->ceilingHeight, vertex[1]);
            v.normal = glm::vec3(0.0f, -1.0f, 0.0f);

            glm::vec3 uAxis = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 vAxis = glm::vec3(0.0f, 0.0f, 1.0f);
            v.texCoord0.s = glm::dot(v.position, uAxis) /* + uOffset */;
            v.texCoord0.t = glm::dot(v.position, vAxis) /* + vOffset */;
            triVertices.push_back(v);
        }
    }

    std::array<Mesh, 2> meshes;
    meshes[1] = CreateMesh(triVertices.data(), (u32)triVertices.size(), indices.data(), (u32)indices.size());

    for (auto& v : triVertices) {
        v.position.y = sector->floorHeight;
        v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 uAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 vAxis = glm::vec3(0.0f, 0.0f, 1.0f);
        v.texCoord0.s = glm::dot(v.position, uAxis) /* + uOffset */;
        v.texCoord0.t = glm::dot(v.position, vAxis) /* + vOffset */;
    }
    std::reverse(indices.begin(), indices.end());
    meshes[0] = CreateMesh(triVertices.data(), (u32)triVertices.size(), indices.data(), (u32)indices.size());
    return meshes;
}

Mesh CreateSectorMesh(const Sector* sector) {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    for (s32 groupIdx = 0; groupIdx < sector->groupCount; ++groupIdx) {
        const SectorGroup* group = &secGroups[sector->firstGroup + groupIdx];
        for (s32 edgeIdx = 0; edgeIdx < group->edgeCount; ++edgeIdx) {
            const Edge* edge = &edges[group->firstEdge + edgeIdx];
            const LineSegment* segment = &segments[edge->seg];
            if (segment->backSector != -1) {
                Sector* backSector = &sectors[segment->backSector];

                if (backSector->floorHeight > sector->floorHeight) {
                    CreateWall(segment->v1, segment->v2, sector->floorHeight, backSector->floorHeight, vertices, indices);
                } else if (backSector->floorHeight < sector->floorHeight) {
                    CreateWall(segment->v2, segment->v1, backSector->floorHeight, sector->floorHeight, vertices, indices);
                }

                if (backSector->ceilingHeight < sector->ceilingHeight) {
                    CreateWall(segment->v1, segment->v2, backSector->ceilingHeight, sector->ceilingHeight, vertices, indices);
                } else if (backSector->ceilingHeight > sector->ceilingHeight) {
                    CreateWall(segment->v2, segment->v1, sector->ceilingHeight, backSector->ceilingHeight, vertices, indices);
                }
                continue;
            }

            CreateWall(segment->v1, segment->v2, sector->floorHeight, sector->ceilingHeight, vertices, indices);
        }
    }

    Mesh mesh = CreateMesh(vertices.data(), (u32)vertices.size(), indices.data(), (u32)indices.size());
    return mesh;
}

int main(int argc, char** argv)
{
    Platform_InitWindow("Gunman", 1920, 1080);
    //Platform_PlayAudioClip(audio, true);

    Input_Init();

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

    Editor_Init();

    TextureId checkerBoardTex = Asset_LoadTexture("textures/blockout/gray_check.png");
    RHI_SetTextureWrapMode(checkerBoardTex, TextureWrapMode_Repeat, TextureWrapMode_Repeat);

    Material sectorMaterial = {};
    sectorMaterial.shader = RHI_CreateShader(R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aNormal;
        layout (location = 3) in vec2 aTexCoord;

        out vec3 vNormal;
        out vec2 vTexCoord;

        uniform mat4 uProjectionMatrix;
        uniform mat4 uViewMatrix;

        void main() {
            gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPosition, 1.0);
            vNormal = abs(aNormal);
            vTexCoord = aTexCoord;
        }
    )", R"(
        #version 330 core
        in vec3 vNormal;
        in vec2 vTexCoord;
        out vec4 fragColor;

        uniform sampler2D uDiffuseTexture;

        void main() {
            float gray = dot(vNormal, vec3(0.89, 0.95, 0.76));
            fragColor = texture(uDiffuseTexture, vTexCoord) * vec4(vec3(gray), 1.0);
        }
    )");
    //sectorMaterial.shader = Asset_LoadShader("shaders/default.vs", "shaders/default.fs");
    sectorMaterial.diffuseTexture = checkerBoardTex;
    sectorMaterial.backfaceCulling = true;
    sectorMaterial.diffuseColor = glm::vec3(1.0f);
    sectorMaterial.useLightmap = false;

    bool toggleTo3D = false;
    bool debugDraw = false;
    bool editorMode = true;

    Camera2D cam = CreateDefaultCamera2D(1280, 720);
    cam.pixelsPerUnit = 32.0f;

    Mesh sectorMeshes[sectorCount * 3];
    for (s32 i = 0; i < sectorCount; ++i) {
        sectorMeshes[i] = CreateSectorMesh(&sectors[i]);
        std::array<Mesh, 2> floorCeilingMeshes = CreateFloorAndCeiling(&sectors[i]);
        sectorMeshes[i + sectorCount] = floorCeilingMeshes[0];
        sectorMeshes[i + sectorCount * 2] = floorCeilingMeshes[1];
    }

    Light light = {};
    light.position = glm::vec3(2.0f, 2.0f, 2.0f);
    light.color = glm::vec3(1.0f, 0.95f, 0.8f);
    light.intensity = 1.0f;
    light.range = 5.0f;

    while (!Platform_WindowShouldClose())
    {
        Input_NextFrame();
        Platform_PollEvents();
        Audio_Update();

        Arena_Clear(&transientStorage);

        if (IsKeyPressed(Key_F3)) {
            editorMode = !editorMode;
        }

        static f64 lastTime = Platform_GetTime();
        f64 currentTime = Platform_GetTime();
        f32 deltaTime = (f32)(currentTime - lastTime);
        lastTime = currentTime;


        if (editorMode) {
            Editor_UpdateAndRender(deltaTime);
        } else {
            if (IsKeyPressed(Key_F1)) {
                toggleTo3D = !toggleTo3D;
                Platform_SetMouseCaptured(toggleTo3D);
            }

            if (IsKeyPressed(Key_F2)) {
                debugDraw = !debugDraw;
            }


            UpdateCameraControls(&controls, &cameraTransform, deltaTime);


            if (toggleTo3D) {
                glm::mat4 projection = Camera_GetProjectionMatrix(&camera);
                glm::mat4 view = Transform_GetMatrixInv(&cameraTransform);
                Renderer_BeginFrame(projection, view);

                Renderer_AddLight(&light);

                for (s32 i = 0; i < sectorCount * 3; ++i) {
                    Mesh* mesh = &sectorMeshes[i];
                    Renderer_DrawMesh(mesh, &sectorMaterial, glm::mat4(1.0f));

                    if (debugDraw) {
                        Renderer_DrawBox(mesh->aabb.min, mesh->aabb.max, Color_ConvertToVec4(COLOR_GREEN));
                    }
                }
                Renderer_EndFrame();

            } else { 
                f32 hw = cam.viewportSize.x * 0.5f;
                f32 hh = cam.viewportSize.y * 0.5f;
                Camera camera = Camera_CreateOrthographic(-hw, hw, hh, -hh, -1.0f, 1.0f);
                glm::mat4 projection = Camera_GetProjectionMatrix(&camera);
                glm::mat4 view = Camera2D_GetViewMatrix(&cam);
                Renderer2D_BeginFrame(&projection, &view);

                for (s32 i = 0; i < sectorCount; ++i) {
                    DrawSector(&sectors[i]);
                }

                /*
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
                */


                // Origin
                glm::vec2 origin = glm::vec2(0.0f, 0.0f);
                glm::vec2 xAxis = glm::vec2(5.0f, 0.0f);
                glm::vec2 yAxis = glm::vec2(0.0f, 5.0f);

                Renderer2D_DrawLine(origin, origin + xAxis, COLOR_RED);
                Renderer2D_DrawLine(origin, origin + yAxis, COLOR_GREEN);


                Renderer2D_EndFrame();
            }
        }
        Platform_SwapBuffers();
    }

    RHI_Shutdown();
    Audio_Shutdown();
    return 0;
}