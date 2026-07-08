#include "Enemy.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Fireball.h"
#include "Koopa.h"
#include <iostream>

Enemy::Enemy(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor)
    : DynamicEntity(pos, sprSize, hitSize, hitOffset, texID, dbgColor) {
    aiStrategy = new PatrolHorizontalStrategy();
}

Enemy::~Enemy() {
    delete aiStrategy;
}

void Enemy::update(float dt) {
    if (aiStrategy) {
        aiStrategy->updateAI(*this, dt);
    }
}

void Enemy::takeDamage() {
    std::cout << "[DEBUG] Enemy (" << textureID << ") took damage at position (" << position.x << ", " << position.y << ")" << std::endl;
    active = false;
    EventManager::getInstance().broadcast(EventType::EnemyStomped);
}

void Enemy::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive()) return;
    if (Player* player = dynamic_cast<Player*>(&other)) {
        if (side == CollisionSide::Top) {
            std::cout << "[DEBUG]   -> Case: Player stomped Enemy. Enemy taking damage." << std::endl;
            takeDamage();
        } else {
            std::cout << "[DEBUG]   -> Case: Player collided on side/bottom. Player taking damage." << std::endl;
            player->takeDamage();
        }
    } 
    else if (other.isSolid()) {
        if (side == CollisionSide::Left || side == CollisionSide::Right) {
            //std::cout << "[DEBUG]   -> Case: Solid block side collision. Reversing walking direction." << std::endl;
            velocity.x = -velocity.x;
            facingRight = (velocity.x > 0.0f);
        }
    }
    else if (Fireball* fireball = dynamic_cast<Fireball*>(&other))
    {
        (void)fireball;
        std::cout << "[DEBUG]   -> Case: Collided with Fireball. Enemy taking damage." << std::endl;
        takeDamage();
    }
    else if (Koopa* koopa = dynamic_cast<Koopa*>(&other))
    {
        if (koopa->isInShell() && koopa->isShellMoving())
        {
            std::cout << "[DEBUG]   -> Case: Collided with moving Koopa shell. Enemy taking damage." << std::endl;
            takeDamage();
        }
    }
}
