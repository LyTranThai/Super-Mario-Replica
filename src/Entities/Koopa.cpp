#include "Koopa.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Core/GameEngine.h"
#include <iostream>

Koopa::Koopa(Vector2 pos)
    : Enemy(pos, Vector2{ 32.0f, 48.0f }, Vector2{ 24.0f, 40.0f }, Vector2{ 4.0f, 8.0f }, "koopa", GREEN),
      inShell(false), shellMoving(false), carried(false) {}

void Koopa::update(float dt) {
    if (carried) {
        velocity.y = 0.0f;
        velocity.x = 0.0f;
        return;
    }
    if (inShell) {
        if (!shellMoving) {
            velocity.x = 0.0f;
        } else {
            // Shell is moving rapidly, let physics carry it
            velocity.x = facingRight ? 400.0f : -400.0f;
        }
    } else {
        // Walk AI
        Enemy::update(dt);
    }
}

void Koopa::takeDamage() {
    if (!inShell) {
        inShell = true;
        shellMoving = false;
        velocity.x = 0.0f;
        
        // Resize Koopa to represent a shell
        spriteSize = Vector2{ 32.0f, 32.0f };
        hitboxSize = Vector2{ 24.0f, 24.0f };
        hitboxOffset = Vector2{ 4.0f, 8.0f };
        textureID = "koopa_shell";
        
        EventManager::getInstance().broadcast(EventType::EnemyStomped);
    } else {
        // If already in shell, toggle slide on stomp
        shellMoving = !shellMoving;
        if (shellMoving) {
            velocity.x = facingRight ? 400.0f : -400.0f;
        } else {
            velocity.x = 0.0f;
        }
    }
}

void Koopa::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive() || carried) return;

    Player* player = dynamic_cast<Player*>(&other);
    if (player) {
        if (side == CollisionSide::Top) {
            takeDamage();
            // Bounce player
            player->setVelocity(Vector2{ player->getVelocity().x, -350.0f });
        } else {
            if (inShell && !shellMoving) {
                if (GameEngine::getInstance().getInputManager().isActionPressed(Action::Run)) {
                    carried = true;
                    player->setCarriedEntity(this);
                } else {
                    // Kick the shell!
                    shellMoving = true;
                    facingRight = (player->getPosition().x < position.x);
                    velocity.x = facingRight ? 400.0f : -400.0f;
                    // Position offset slightly to prevent immediate recollision
                    Vector2 pos = position;
                    pos.x += facingRight ? 10.0f : -10.0f;
                    setPosition(pos);
                    EventManager::getInstance().broadcast(EventType::PlayerJump); // Play kick audio
                }
            } else {
                // Get carried
                carried = true;
                player->setCarriedEntity(this);
            }
        }
    } 
    if (other.isSolid()) {
        if (side == CollisionSide::Left || side == CollisionSide::Right) {
            velocity.x = -velocity.x;
            facingRight = (velocity.x > 0.0f);
        }
    }
    if (Enemy* enemy = dynamic_cast<Enemy*>(&other)) {
        // Colliding with another enemy while moving as a shell deals damage to them
        if (inShell && shellMoving) {
            enemy->takeDamage();
        }
    }
}
