#include "game.h"
#include "renderer/renderer.h"
#include "renderer/renderer_2d.h"
#include "core/platform.h"

#include <glm/gtc/matrix_transform.hpp>

static void HandleLevelCollisionOnAxis(const Level* level, Entity* entity, glm::vec2 axis)
{
    TransformComponent* transform = &entity->transform;
    MovementComponent* movement = &entity->movement;

    glm::vec2 position = glm::vec2(transform->position.x, transform->position.z);

    f32 radius = entity->collider.radius;
    u32 minX = (u32)(position.x - radius);
    u32 maxX = (u32)(position.x + radius);
    u32 minY = (u32)(position.y - radius);
    u32 maxY = (u32)(position.y + radius);

    for (u32 y = minY; y <= maxY; ++y)
    {
        for (u32 x = minX; x <= maxX; ++x)
        {
            Tile* tile = Level_GetTileAt(level, x, y);

            if (!tile || !tile->solid) continue;

            Box2 tileBox = CreateBox2(glm::vec2(x, y), glm::vec2(x + 1, y + 1));

            if (Box2_CircleIntersect(&tileBox, position, radius))
            {
                glm::vec2 closestPoint = Box2_ClosestPoint(&tileBox, position);
                glm::vec2 direction = Normalize(position - closestPoint);

                if (axis.x == 1.0f)
                {
                    position.x = closestPoint.x + direction.x * radius;
                }
                else if (axis.y == 1.0f)
                {
                    position.y = closestPoint.y + direction.y * radius;
                }
            }
            transform->position.x = position.x;
            transform->position.z = position.y;
        }
    }
}

static void ApplyFriction(MovementComponent* movement)
{
    if (movement->applyFriction)
    {
        movement->velocity *= (1.0f - movement->friction);

        if (Abs(movement->velocity.x) < 1e-4f) movement->velocity.x = 0.0f;
        if (Abs(movement->velocity.y) < 1e-4f) movement->velocity.y = 0.0f;
        if (Abs(movement->velocity.z) < 1e-4f) movement->velocity.z = 0.0f;
    }
}

void MovementSystem(Entity** entities, u32 entityCount, Level* level, f32 deltaTime)
{
    for (u32 i = 0; i < entityCount; ++i)
    {
        Entity* entity = entities[i];

        if (!entity->transform.enabled || !entity->movement.enabled) continue;

        glm::vec3 lastPosition = entity->transform.position;

        ApplyFriction(&entity->movement);

        if (entity->collider.enabled)
        {
            entity->transform.position.x += entity->movement.velocity.x * deltaTime;
            HandleLevelCollisionOnAxis(level, entity, glm::vec2(1, 0));

            entity->transform.position.z += entity->movement.velocity.z * deltaTime;
            HandleLevelCollisionOnAxis(level, entity, glm::vec2(0, 1));
        }

        if (lastPosition.x != entity->transform.position.x ||
            lastPosition.z != entity->transform.position.z)
        {
            //Level_UpdateEntityPosition(level, entity, lastPosition);
        }
    }
}

void CollisionSystem(Entity** entities, u32 entityCount, f32 deltaTime)
{
    for (u32 i = 0; i < entityCount - 1; ++i)
    {
        for (u32 j = i + 1; j < entityCount; ++j)
        {
            Entity* entityA = entities[i];
            Entity* entityB = entities[j];

            if (entityB->collider.type == ColliderType_Sphere)
            {
                entityA = entities[j];
                entityB = entities[i];
            }

            if (entityA->collider.enabled && entityB->collider.enabled)
            {
                if (entityA->collider.type == ColliderType_Sphere &&
                    entityB->collider.type == ColliderType_Box)
                {
                    Box3 aabb = entityB->collider.aabb;
                    Box3_Translate(&aabb, entityB->transform.position);
                    if (Box3_SphereIntersect(&aabb, entityA->transform.position, entityA->collider.radius))
                    {
                        glm::vec3 closestPoint = Box3_ClosestPoint(&aabb, entityA->transform.position);
                        glm::vec3 direction = Normalize(entityA->transform.position - closestPoint);

                        if (entityB->collider.isTrigger)
                        {
                            if (entityB->collider.onTriggerEnter && !entityB->collider.currentEntity)
                            {
                                entityB->collider.currentEntity = entityA;
                                entityB->collider.onTriggerEnter(game->context, entityB, entityA);
                            }
                        }
                        else
                        {
                            entityA->transform.position = closestPoint + direction * entityA->collider.radius;
                        }
                    }
                    else
                    {
                        if (entityB->collider.isTrigger && entityB->collider.currentEntity == entityA)
                        {
                            entityB->collider.currentEntity = nullptr;
                            if (entityB->collider.onTriggerExit)
                            {
                                entityB->collider.onTriggerExit(game->context, entityB, entityA);
                            }
                        }
                    }
                }
                else if (entityA->collider.type == ColliderType_Sphere &&
                         entityB->collider.type == ColliderType_Sphere)
                {
                    glm::vec3 centerA = entityA->transform.position;
                    glm::vec3 centerB = entityB->transform.position;
                    f32 radiusA = entityA->collider.radius;
                    f32 radiusB = entityB->collider.radius;
                    if (SpheresIntersect(centerA, radiusA, centerB, radiusB))
                    {
                        glm::vec3 direction = glm::normalize(centerA - centerB);
                        f32 distance = glm::length(centerA - centerB);
                    }
                }
            }
        }
    }
}

void AnimationSystem(Entity** entities, u32 entityCount, f32 deltaTime)
{
    for (u32 i = 0; i < entityCount; ++i)
    {
        Entity* entity = entities[i];

        if (!entity->spriteAnimation.enabled || !entity->meshRenderer.enabled) continue;

        SpriteAnimationComponent* spriteAnimationComponent = &entity->spriteAnimation;
        SpriteAnimation* animation = spriteAnimationComponent->animation;

        if (animation == nullptr || animation->frameCount == 0) continue;
        
        spriteAnimationComponent->timer += deltaTime;

        if (spriteAnimationComponent->timer >= animation->frameTime)
        {
            spriteAnimationComponent->timer = 0.0f;
            spriteAnimationComponent->currentFrame++;

            if (spriteAnimationComponent->currentFrame >= animation->frameCount)
            {
                if (animation->loop)
                {
                    spriteAnimationComponent->currentFrame = 0;
                }
                else
                {
                    spriteAnimationComponent->currentFrame = animation->frameCount - 1;
                }
            }
        }

        if (spriteAnimationComponent->currentFrame < animation->frameCount)
        {
            Sprite* sprite = animation->sprites[spriteAnimationComponent->currentFrame];

            MeshRendererComponent* meshRendererComponent = &entity->meshRenderer;
            meshRendererComponent->material->diffuseTexture = sprite->texture;
        }
    }
}

void RenderSystem(Entity** entities, u32 entityCount)
{
    Entity* entityWithCamera = nullptr;
    for (u32 i = 0; i < entityCount; ++i)
    {
        if (entities[i]->camera.enabled)
        {
            entityWithCamera = entities[i];
            break;
        }
    }

    if (entityWithCamera == nullptr) return;

    CameraComponent* camera = &entityWithCamera->camera;
    glm::mat4 projectionMatrix = glm::perspective(camera->fov, camera->aspect, camera->nearClip, camera->farClip);
    glm::mat4 viewMatrix = glm::inverse(entityWithCamera->transform.GetTransformMatrixNoScale());

    Renderer_BeginFrame(projectionMatrix, viewMatrix);
    
    for (u32 i = 0; i < entityCount; ++i)
    {
        Entity* entity = entities[i];

        if (entity->type == EntityType_Light)
        {
            Light light = {};
            light.position = entity->transform.position;
            light.intensity = entity->light.intensity;
            light.color = entity->light.color;
            light.range = entity->light.range;
            
            Renderer_AddLight(&light);
        }

        if (entity->transform.enabled && entity->meshRenderer.enabled)
        {
            Mesh* mesh = entity->meshRenderer.mesh;
            Material* material = entity->meshRenderer.material;
            Renderer_DrawMesh(mesh, material, entity->transform.GetTransformMatrix());
        }

        if (entity->collider.enabled && game->settings.debugMode)
        {
            if (entity->collider.type == ColliderType_Box)
            {
                Box3 aabb = entity->collider.aabb;
                Box3_Translate(&aabb, entity->transform.position + entity->collider.offset);
                Renderer_DrawBox(aabb.min, aabb.max);
            }
            else if (entity->collider.type == ColliderType_Sphere)
            {
                Renderer_DrawSphere(entity->transform.position + entity->collider.offset, entity->collider.radius, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
        }
    }

    //DrawPlayer(game->context, game->context->player);

    Renderer_EndFrame();

    Renderer2D_BeginFrame();

    f32 windowCenterX = (f32)Platform_GetWindowWidth() / 2.0f;
    f32 windowCenterY = (f32)Platform_GetWindowHeight() / 2.0f;
    f32 spriteWidth = 32.0f;
    f32 spriteHeight = 32.0f;
    glm::vec2 position = glm::vec2(windowCenterX - spriteWidth * 0.5f, windowCenterY - spriteHeight * 0.5f);
    glm::vec2 size = glm::vec2(spriteWidth, spriteHeight);

    Renderer2D_DrawRect(position, size, COLOR_WHITE);

    Renderer2D_EndFrame();
}