#include "RockHead.h"
#include "Player.h"
#include <cmath>
#include <iostream>

RockHead::RockHead(Vector2 pos)
    : Enemy(pos, Vector2{ 48.0f, 64.0f }, Vector2{ 40.0f, 56.0f }, Vector2{ 4.0f, 4.0f }, "thwomp", DARKGRAY),
      rockState(RockState::Idle), timer(0.0f), spawnPosition(pos) {
    delete aiStrategy;
    aiStrategy = nullptr;
    velocity = Vector2{ 0.0f, 0.0f };
    onGround = true; // Overrides default gravity
}

void RockHead::update(float dt) {
    switch (rockState) {
        case RockState::Idle:
            velocity = Vector2{ 0.0f, 0.0f };
            break;
        case RockState::Slamming:
            // Accelerate down rapidly
            velocity.y += 1200.0f * dt;
            if (velocity.y > 600.0f) {
                velocity.y = 600.0f;
            }
            break;
        case RockState::Waiting:
            velocity = Vector2{ 0.0f, 0.0f };
            timer -= dt;
            if (timer <= 0.0f) {
                rockState = RockState::Rising;
            }
            break;
        case RockState::Rising:
            velocity.y = -80.0f;
            if (position.y <= spawnPosition.y) {
                position.y = spawnPosition.y;
                velocity.y = 0.0f;
                rockState = RockState::Idle;
            }
            break;
    }
}

void RockHead::checkTrigger(Vector2 playerPos) {
    if (rockState == RockState::Idle) {
        float diffX = fabsf(playerPos.x - position.x);
        // Trigger slam if player is directly below
        if (diffX < 80.0f && playerPos.y > position.y) {
            rockState = RockState::Slamming;
            velocity.y = 150.0f;
            onGround = false;
        }
    }
}

void RockHead::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive()) return;

    
    Player* player = dynamic_cast<Player*>(&other);
    if (player) {
        std::cout << "[DEBUG]   -> Case: Player touched RockHead. Player taking damage." << std::endl;
        player->takeDamage();
    } 
    else if (other.isSolid()) {
        if (side == CollisionSide::Bottom && rockState == RockState::Slamming) {
            //std::cout << "[DEBUG]   -> Case: Slamming hit bottom solid block. Transitioning to Waiting state." << std::endl;
            rockState = RockState::Waiting;
            timer = 1.0f; // Stay down for 1 second
            velocity.y = 0.0f;
        }
    }
}
