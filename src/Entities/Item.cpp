#include "Item.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "SpriteAnimator.h"
#include <iostream>

Item::Item(Vector2 pos, ItemType type, const std::string& texID, Color dbgColor)
    : DynamicEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 24.0f, 24.0f }, Vector2{ 4.0f, 4.0f }, texID, dbgColor),
      itemType(type), spawnRiseTimer(0.5f), targetSpawnPosition(pos) {
    
    // Slide up starting position slightly hidden inside the block
    position.y += 16.0f;
    velocity = Vector2{ 0.0f, 0.0f };
    onGround = true; // Stay stationary during spawn rising
    
    animator = std::make_unique<SpriteAnimator>();
    if (type == ItemType::Mushroom) {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{192, 32, 16, 16}}, 1.0f);
    } else if (type == ItemType::FireFlower) {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{0, 64, 16, 16}, {16, 64, 16, 16}, {32, 64, 16, 16}, {48, 64, 16, 16}}, 10.0f);
    } else if (type == ItemType::Star) {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{0, 96, 16, 16}, {16, 96, 16, 16}, {32, 96, 16, 16}, {48, 96, 16, 16}}, 10.0f);
    } else if (type == ItemType::Coin) {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{160, 32, 16, 16}, {176, 32, 16, 16}, {192, 32, 16, 16}, {208, 32, 16, 16}}, 10.0f);
    } else {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{0, 0, 16, 16}}, 1.0f);
    }
    static_cast<SpriteAnimator*>(animator.get())->setState("idle");
}

void Item::update(float dt) {
    if (animator) animator->update(dt);
    
    if (spawnRiseTimer > 0.0f) {
        spawnRiseTimer -= dt;
        // Slowly rise out of block
        position.y -= 32.0f * dt / 0.5f;
        if (spawnRiseTimer <= 0.0f) {
            position.y = targetSpawnPosition.y - 32.0f; // Snap to top of block
            onGround = false;
            if (itemType == ItemType::Mushroom) {
                velocity.x = 80.0f;
                facingRight = true;
            } else if (itemType == ItemType::Star) {
                velocity.x = 120.0f;
                facingRight = true;
                velocity.y = -200.0f;
            }
        }
    } else {
        // Post spawn behaviors
        if (itemType == ItemType::Mushroom) {
            velocity.x = facingRight ? 80.0f : -80.0f;
            // Let gravity apply
        } 
        else if (itemType == ItemType::FireFlower) {
            velocity = Vector2{ 0.0f, 0.0f };
            onGround = true; // No physics
        } 
        else if (itemType == ItemType::Star) {
            velocity.x = facingRight ? 120.0f : -120.0f;
            if (onGround) {
                velocity.y = -250.0f; // Bounce!
                onGround = false;
            }
        }
    }
}

void Item::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive()) return;

    Player* player = dynamic_cast<Player*>(&other);
    if (player) {
        // Collected by player!
        active = false;
        if (itemType == ItemType::Mushroom) {
            player->powerUp(PowerStateType::Super);
            EventManager::getInstance().broadcast(EventType::CoinCollected); // Reuse coin sound for simple powerups
        } 
        else if (itemType == ItemType::FireFlower) {
            player->powerUp(PowerStateType::Fire);
            EventManager::getInstance().broadcast(EventType::CoinCollected);
        } 
        else if (itemType == ItemType::Star) {
            // Invincibility star
            player->addScore(1000);
            // Toggle invincibility in player
            // We set long invincibility frames (e.g. 10 seconds)
            player->takeDamage(); // Set invincibility directly
            EventManager::getInstance().broadcast(EventType::CoinCollected);
        }
    } 
    else if (other.isSolid()) {
        if (side == CollisionSide::Left || side == CollisionSide::Right) {
            velocity.x = -velocity.x;
            facingRight = (velocity.x > 0.0f);
        }
    }
}
