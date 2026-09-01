#include "RockHead.h"
#include "Player.h"
#include "Core/AssetManager.h"
#include <cmath>
#include <iostream>

RockHead::RockHead(Vector2 pos)
    : Enemy(pos, Vector2{ 48.0f, 64.0f }, Vector2{ 40.0f, 56.0f }, Vector2{ 4.0f, 4.0f }, "enemy", DARKGRAY),
      rockState(RockState::Idle), timer(0.0f), spawnPosition(pos) {
    delete aiStrategy;
    aiStrategy = nullptr;
    velocity = Vector2{ 0.0f, 0.0f };
    onGround = true; // Overrides default gravity

    animator.addAnimation(AnimState::Idle, {
        Rectangle{ 273.0f, 37.0f, 16.0f, 24.0f }
    }, 1.0f);
    
    animator.addAnimation(AnimState::Fall, {
        Rectangle{ 290.0f, 37.0f, 16.0f, 24.0f }
    }, 1.0f);
    
    animator.setState(AnimState::Idle);
}

void RockHead::update(float dt) {
    switch (rockState) {
        case RockState::Idle:
            velocity = Vector2{ 0.0f, 0.0f };
            position.y = spawnPosition.y; // Ensure rockhead stays firmly anchored at its sky position
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

    // Safety: deactivate if fell deep into a pit
    if (position.y > 1000.0f) {
        active = false;
    }

    if (rockState == RockState::Slamming) {
        animator.setState(AnimState::Fall);
    } else {
        animator.setState(AnimState::Idle);
    }
    animator.update(dt);
}

void RockHead::applyGravity(float dt) {
    // RockHead manages its own kinematics (stationary hover in Idle, rapid drop in Slamming,
    // and steady ascent in Rising). General physics gravity must NOT be applied.
    (void)dt;
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
            rockState = RockState::Waiting;
            timer = 1.0f; // Stay down for 1 second
            velocity.y = 0.0f;
        } else if (side == CollisionSide::Top && rockState == RockState::Rising) {
            // Hit a ceiling or solid block above while rising
            rockState = RockState::Idle;
            spawnPosition.y = position.y;
            velocity.y = 0.0f;
        }
    }
}

void RockHead::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle source = animator.getCurrentFrame();
        
        float scale = 3.0f; // Scale 16x24 to approx 48x64
        float destWidth = std::abs(source.width) * scale;
        float destHeight = std::abs(source.height) * scale;
        
        float destX = position.x;
        float destY = position.y + (64.0f - destHeight); // Align to bottom
        
        Rectangle dest = { destX, destY, destWidth, destHeight };
        
        if (!facingRight) {
            source.width = -source.width;
        }
        
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    } else {
        Enemy::draw();
    }
}
