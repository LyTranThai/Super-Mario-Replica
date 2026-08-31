#include "Mushroom.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Core/AssetManager.h"
#include <iostream>

Mushroom::Mushroom(Vector2 pos)
    : DynamicEntity(pos, Vector2{32.0f, 32.0f}, Vector2{24.0f, 24.0f}, Vector2{4.0f, 4.0f}, "coin", RED),
      spawnRiseTimer(0.5f), targetSpawnPosition(pos)
{

    // Slide up starting position slightly hidden inside the block
    position.y += 16.0f;
    velocity = Vector2{0.0f, 0.0f};
    onGround = true; // Stay stationary during spawn rising

    animator.addAnimation(AnimState::Idle, {Rectangle{0.0f, 176.0f, 16.0f, 16.0f}, Rectangle{16.0f, 176.0f, 16.0f, 16.0f}}, 0.2f);
    animator.setState(AnimState::Idle);
}

void Mushroom::update(float dt)
{
    animator.update(dt);
    if (spawnRiseTimer > 0.0f)
    {
        spawnRiseTimer -= dt;
        // Slowly rise out of block
        position.y -= 32.0f * dt / 0.5f;
        if (spawnRiseTimer <= 0.0f)
        {
            position.y = targetSpawnPosition.y - 32.0f; // Snap to top of block
            onGround = false;
            velocity.x = 80.0f;
            facingRight = true;
        }
    }
    else
    {
        velocity.x = facingRight ? 80.0f : -80.0f;
    }
}

void Mushroom::onCollision(Entity &other, CollisionSide side)
{
    if (!other.isActive())
        return;

    Player *player = dynamic_cast<Player *>(&other);
    if (player)
    {
        // Collected by player!
        active = false;
        player->powerUp(PowerStateType::Super);
        EventManager::getInstance().broadcast(EventType::PowerUpCollected);
    }
    else if (other.isSolid())
    {
        if (side == CollisionSide::Left || side == CollisionSide::Right)
        {
            velocity.x = -velocity.x;
            facingRight = (velocity.x > 0.0f);
        }
    }
}

void Mushroom::draw()
{
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0)
    {
        Rectangle source = animator.getCurrentFrame();
        Rectangle dest = getSpriteBox();
        DrawTexturePro(tex, source, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    }
    else
    {
        DynamicEntity::draw();
    }
}
