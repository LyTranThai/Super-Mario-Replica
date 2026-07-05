#include "Enemy.h"
#include "Player.h"
#include "Core/EventSystem.h"
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
    active = false;
    EventManager::getInstance().broadcast(EventType::EnemyStomped);
}

void Enemy::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive()) return;

    Player* player = dynamic_cast<Player*>(&other);
    if (player) {
        if (side == CollisionSide::Top) {
            // Player stomped on us
            takeDamage();
        } else {
            // Player collided on side or bottom -> hurts player
            player->takeDamage();
        }
    } 
    else if (other.isSolid()) {
        // Block collision on side -> reverse walking direction
        if (side == CollisionSide::Left || side == CollisionSide::Right) {
            velocity.x = -velocity.x;
            facingRight = (velocity.x > 0.0f);
        }
    }
}
