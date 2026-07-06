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
        
        std::cout << "[DEBUG] Koopa (" << textureID << ") took damage and retreated to shell." << std::endl;
        EventManager::getInstance().broadcast(EventType::EnemyStomped);
    } else {
        // If already in shell, toggle slide on stomp
        shellMoving = !shellMoving;
        if (shellMoving) {
            velocity.x = facingRight ? 400.0f : -400.0f;
        } else {
            velocity.x = 0.0f;
        }
        std::cout << "[DEBUG] Koopa shell took damage. shellMoving toggled to: " << (shellMoving ? "true" : "false") << std::endl;
    }
}

void Koopa::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive() || carried) return;

    
    Player* player = dynamic_cast<Player*>(&other);
    if (player) {
        if (side == CollisionSide::Top) {
            std::cout << "[DEBUG]   -> Case: Player stomped Koopa. Koopa taking damage." << std::endl;
            takeDamage();
            // Bounce player
            player->setVelocity(Vector2{ player->getVelocity().x, -350.0f });
        } else {
            if (inShell && !shellMoving) {
                if (GameEngine::getInstance().getInputManager().isActionPressed(Action::Run)) {
                    std::cout << "[DEBUG]   -> Case: Player carrying Koopa shell." << std::endl;
                    carried = true;
                    player->setCarriedEntity(this);
                } else {
                    // Kick the shell!
                    std::cout << "[DEBUG]   -> Case: Player kicked Koopa shell." << std::endl;
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
                std::cout << "[DEBUG]   -> Case: Player touched Koopa side. Koopa gets carried." << std::endl;
                carried = true;
                player->setCarriedEntity(this);
            }
        }
    } 
    if (other.isSolid()) {
        if (side == CollisionSide::Left || side == CollisionSide::Right) {
            //std::cout << "[DEBUG]   -> Case: Solid block side collision. Reversing direction." << std::endl;
            velocity.x = -velocity.x;
            facingRight = (velocity.x > 0.0f);
        }
    }
    if (Enemy* enemy = dynamic_cast<Enemy*>(&other)) {
        // Colliding with another enemy while moving as a shell deals damage to them
        if (inShell && shellMoving) {
            std::cout << "[DEBUG]   -> Case: Moving shell hit another enemy. Enemy (" << enemy->getTextureID() << ") taking damage." << std::endl;
            enemy->takeDamage();
        }
    }
}
