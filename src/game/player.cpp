#include "game.h"
#include "core/input.h"

void UpdatePlayer(GameContext* context, Entity* entity, f32 deltaTime)
{
    Player* player = &entity->player;

    TransformComponent* transform = &entity->transform;
    MovementComponent* movement = &entity->movement;

    v3 forward = transform->GetForward();
    v3 right = transform->GetRight();

    // Restrict the forward and right vectors to the horizontal plane
    forward.y = 0.0f;
    right.y = 0.0f;
    forward = glm::normalize(forward);
    right = glm::normalize(right);

    if (IsKeyDown(Key_W) + IsKeyDown(Key_Up))
    {
        movement->velocity += forward * player->speed * deltaTime;
    }

    if (IsKeyDown(Key_S) + IsKeyDown(Key_Down))
    {
        movement->velocity -= forward * player->speed * deltaTime;
    }

    if (IsKeyDown(Key_A))
    {
        movement->velocity -= right * player->speed * deltaTime;
    }

    if (IsKeyDown(Key_D))
    {
        movement->velocity += right * player->speed * deltaTime;
    }

    if (IsKeyDown(Key_Left))
    {
        transform->rotation.y += 90.0f * deltaTime;
    }

    if (IsKeyDown(Key_Right))
    {
        transform->rotation.y -= 90.0f * deltaTime;
    }

    // Update player rotation based on mouse movement
    glm::vec2 mouseDelta = GetRelativeMousePosition();
    transform->rotation.x -= mouseDelta.y * game->settings.mouseSensitivity;
    transform->rotation.y -= mouseDelta.x * game->settings.mouseSensitivity;

    // Clamp vertical rotation to prevent flipping
    transform->rotation.x = glm::clamp(transform->rotation.x, -89.0f, 89.0f);

    f32 localVelocityX = glm::dot(movement->velocity, transform->GetRight());
    entity->transform.rotation.z = -localVelocityX;

    player->pointLight->transform.position = transform->position;
}

b32 Player_HasKey(const Entity* player, u32 key)
{
    /*
    const Player* data = &player->data.player;
    for (s32 i = 0; i < data->keyCount; ++i)
    {
        if ((data->keys & key) == key)
        {
            return true;
        }
    }
    */
    return false;
}

void Player_AddKey(Entity* player, u32 keyId)
{
    /*
    Player* data = &player->data.player;
    if (data->keyCount < MAX_KEYS)
    {
        data->keys |= keyId;
    }*/
}


void DrawPlayer(GameContext* context, Entity* entity)
{
    Level* level = context->level;

    v3 forwardVector = entity->transform.GetForward();
    v2 forward2D = Normalize(v2(forwardVector.x, forwardVector.z));

    RayCastHit hit;

    if (Level_CastRay(level, glm::vec2(entity->transform.position.x, entity->transform.position.z), forward2D, &hit))
    {
        v3 hitPosition = entity->transform.position + Normalize(v3(forward2D.x, 0.0f, forward2D.y)) * hit.distance;
        Box3 box = {};
        box.min = v3(-0.05f);
        box.max = v3(0.05f);
        Box3_Translate(&box, hitPosition);
        Renderer_DrawBox(box.min, box.max, glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
    }
}