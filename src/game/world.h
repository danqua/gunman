#pragma once
#include "core/types.h"
#include "core/math.h"
#include "renderer/renderer.h"
#include "renderer/sprite.h"

#define MAX_WORLD_ENTITIES 128

struct GameContext;
struct Entity;

struct TransformComponent
{
    bool enabled;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    glm::mat4 GetTransformMatrix() const;
    glm::mat4 GetTransformMatrixNoScale() const;
};

struct MovementComponent
{
    bool enabled;
    glm::vec3 velocity;
    f32 friction;
    bool applyFriction;
};

enum ColliderType
{
    ColliderType_None = 0,
    ColliderType_Box,
    ColliderType_Sphere
};

typedef void(*TriggerCallback)(GameContext*, Entity*, Entity*);

struct ColliderComponent
{
    bool enabled;
    ColliderType type;

    union
    {
        f32 radius;
        Box3 aabb;
    };
    glm::vec3 offset;

    TriggerCallback onTriggerEnter;
    TriggerCallback onTriggerExit;
    TriggerCallback onTriggerStay;
    bool isTrigger;
    Entity* currentEntity;
};

struct MeshRendererComponent
{
    bool enabled;
    Mesh* mesh;
    Material* material;
};

struct SpriteAnimationComponent
{
    bool enabled;
    f32 timer;
    u32 currentFrame;
    SpriteAnimation* animation;
};

enum LightType
{
    LightType_Point,
    LightType_Directional
};

struct LightComponent
{
    bool enabled;
    LightType type;
    glm::vec3 color;
    f32 intensity;
    f32 range;
};

struct CameraComponent
{
    bool enabled;
    f32 fov;
    f32 aspect;
    f32 nearClip;
    f32 farClip;
};