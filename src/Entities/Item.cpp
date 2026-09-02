#include "Item.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Core/AssetManager.h"
#include <iostream>

Item::Item(Vector2 pos, const std::string& texID, Color dbgColor)
    : DynamicEntity(pos, Vector2{32.0f, 32.0f}, Vector2{24.0f, 24.0f}, Vector2{4.0f, 4.0f}, texID, dbgColor),
      spawnRiseTimer(1.0f), targetSpawnPosition({pos.x, pos.y - 32.0f})
{
    position = pos;
    velocity = Vector2{0.0f, 0.0f};
    onGround = true; // Stay stationary during spawn rising
    riseSpeed = 16.0f / 1;
}

void Item::update(float dt)
{
    animator.update(dt);
    if (spawnRiseTimer > 0.0f)
    {
        spawnRiseTimer -= dt;
        // Slowly rise out of block
        position.y -= riseSpeed * dt;
        if (spawnRiseTimer <= 0.0f)
        {
            // position.y = targetSpawnPosition.y; // Snap exactly to target
            onGround = false;
            //velocity.y = -150.0f; // Small pop out hop!
            onSpawnComplete();
        }
    }
    else
    {
        updateBehavior(dt);
    }
}

void Item::onCollision(Entity &other, CollisionSide side)
{
    if (!other.isActive())
        return;

    Player *player = dynamic_cast<Player *>(&other);
    if (player)
    {
        // Collected by player!
        active = false;
        applyEffect(player);
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

void Item::draw()
{
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0)
    {
        Rectangle source = animator.getCurrentFrame();
        // If the item doesn't have animation frames (like static texture), just draw it
        if (source.width == 0) {
            DynamicEntity::draw();
        } else {
            Rectangle dest = getSpriteBox();
            DrawTexturePro(tex, source, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
        }
    }
    else
    {
        DynamicEntity::draw();
    }
}
