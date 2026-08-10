#include "InteractiveBlock.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Core/AssetManager.h"
#include "SpriteAnimator.h"
#include <iostream>
#include <cmath>

struct ItemSpawnData {
    Vector2 position;
    ItemType type;
};

InteractiveBlock::InteractiveBlock(Vector2 pos, InteractiveBlockType type, ItemType item)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, 
                   (type == InteractiveBlockType::Question) ? "question" : "brick", 
                   (type == InteractiveBlockType::Question) ? GOLD : ORANGE),
      blockType(type), hiddenItem(item), isUsed(false), bounceTimer(0.0f), originalPosition(pos) {
          
    animator = std::make_unique<SpriteAnimator>();
    if (type == InteractiveBlockType::Question) {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{0, 0, 16, 16}, {16, 0, 16, 16}, {32, 0, 16, 16}}, 5.0f);
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("used", {{48, 0, 16, 16}}, 1.0f);
    } else {
        static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{80, 0, 16, 16}}, 1.0f);
    }
    static_cast<SpriteAnimator*>(animator.get())->setState("idle");
}

void InteractiveBlock::update(float dt) {
    if (animator) animator->update(dt);
    
    if (bounceTimer > 0.0f) {
        bounceTimer -= dt;
        
        // Simple cosine offset for visual bounce animation
        float offset = sinf((bounceTimer / 0.15f) * PI) * 8.0f;
        position.y = originalPosition.y - offset;

        if (bounceTimer <= 0.0f) {
            position.y = originalPosition.y;
        }
    }
}


void InteractiveBlock::onInteract(Player& player) {
    hit(player);
}

void InteractiveBlock::hit(Player& player) {
    if (blockType == InteractiveBlockType::Brick) {
        if (player.getPowerType() != PowerStateType::Small) {
            // Shatter brick block
            active = false;
            EventManager::getInstance().broadcast(EventType::BrickBroken);
        } else {
            // Bounce brick block without breaking
            if (bounceTimer <= 0.0f) {
                bounceTimer = 0.15f;
            }
        }
    } 
    else if (blockType == InteractiveBlockType::Question) {
        if (!isUsed) {
            isUsed = true;
            bounceTimer = 0.15f;
            static_cast<SpriteAnimator*>(animator.get())->setState("used");
            
            if (hiddenItem == ItemType::Coin) {
                player.addCoin();
                player.addScore(200);
                EventManager::getInstance().broadcast(EventType::CoinCollected);
            } 
            else if (hiddenItem != ItemType::None) {
                // Spawn item popping out upward
                Vector2 spawnPos = { originalPosition.x, originalPosition.y - 32.0f };
                ItemSpawnData spawnData = { spawnPos, hiddenItem };
                EventManager::getInstance().broadcast(EventType::ItemSpawned, &spawnData);
            }
        }
    }
}
