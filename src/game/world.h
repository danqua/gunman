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
    b32 enabled;
    v3 position;
    v3 rotation;
    v3 scale;

    v3 GetForward() const;
    v3 GetRight() const;
    v3 GetUp() const;

    m4x4 GetTransformMatrix() const;
    m4x4 GetTransformMatrixNoScale() const;
};

struct MovementComponent
{
    b32 enabled;
    v3 velocity;
    f32 friction;
    b32 applyFriction;
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
    b32 enabled;
    ColliderType type;

    union
    {
        f32 radius;
        Box3 aabb;
    };
    v3 offset;

    TriggerCallback onTriggerEnter;
    TriggerCallback onTriggerExit;
    TriggerCallback onTriggerStay;
    b32 isTrigger;
    Entity* currentEntity;
};

struct MeshRendererComponent
{
    b32 enabled;
    Mesh* mesh;
    Material* material;
};

struct SpriteAnimationComponent
{
    b32 enabled;
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
    b32 enabled;
    LightType type;
    v3 color;
    f32 intensity;
    f32 range;
};

struct CameraComponent
{
    b32 enabled;
    f32 fov;
    f32 aspect;
    f32 nearClip;
    f32 farClip;
};