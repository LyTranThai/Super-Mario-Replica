#include "Koopa.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Core/GameEngine.h"
#include "Fireball.h"
#include "Core/AssetManager.h"
#include <iostream>
#include <cmath>

Koopa::Koopa(Vector2 pos)
    : Enemy(pos, Vector2{ 32.0f, 48.0f }, Vector2{ 24.0f, 40.0f }, Vector2{ 4.0f, 8.0f }, "enemy", GREEN),
      inShell(false), shellMoving(false), carried(false) {
    
    animator.addAnimation(AnimState::Walk, {
        Rectangle{ 0.0f, 112.0f, 16.0f, 24.0f },
        Rectangle{ 18.0f, 112.0f, 16.0f, 24.0f }
    }, 0.2f);
    animator.addAnimation(AnimState::Crouch, {
        Rectangle{ 72.0f, 120.0f, 16.0f, 16.0f }
    }, 1.0f);
    
    animator.setState(AnimState::Walk);
}

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
    
    animator.update(dt);
}

void Koopa::takeDamage() {
    if (!inShell) {
        inShell = true;
        shellMoving = false;
        velocity.x = 0.0f;
        
        position.y += 16.0f; // Shift down so it stays on ground!
        // Resize Koopa to represent a shell
        spriteSize = Vector2{ 32.0f, 32.0f };
        hitboxSize = Vector2{ 24.0f, 24.0f };
        hitboxOffset = Vector2{ 4.0f, 8.0f };
        
        animator.setState(AnimState::Crouch);
        
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
            //player-> setjumpCount(0);
            player-> setVelocity(Vector2{ player->getVelocity().x, -350.0f });
        } else {
            if (inShell) {
                if(GameEngine::getInstance().getInputManager().isActionPressed(Action::Run))
                {
                    std::cout << "[DEBUG]   -> Case: Player carrying Koopa shell." << std::endl;
                    this -> beingCarried(*player);
                } else {
                    // Kick the shell!
                    std::cout << "[DEBUG]   -> Case: Player kicked Koopa shell." << std::endl;
                    shellMoving = true;
                    facingRight = (player->getPosition().x < position.x);
                    velocity.x = facingRight ? 400.0f : -400.0f;
                    velocity.y = 100.0f;
                    // Position offset slightly to prevent immediate recollision
                    Vector2 pos = position;
                    pos.x += facingRight ? 10.0f : -10.0f;
                    setPosition(pos);
                    EventManager::getInstance().broadcast(EventType::PlayerJump); // Play kick audio
                }
            } else {
                // Hurt Player
                player -> takeDamage();
            }
        }
    } 
    if (other.isSolid()) {
        if (side == CollisionSide::Left) {
            facingRight = true;
        } else if (side == CollisionSide::Right) {
            facingRight = false;
        }
    }

    // Colliding with another enemy while moving as a shell deals damage to them
    if (Enemy* enemy = dynamic_cast<Enemy*>(&other)) {
        if (inShell && shellMoving) {
            std::cout << "[DEBUG]   -> Case: Moving shell hit another enemy. Enemy (" << enemy->getTextureID() << ") taking damage." << std::endl;
            enemy->takeDamage();
        }
    }

    if(Fireball* fireball = dynamic_cast<Fireball*> (&other))
    {
        if(!inShell)
        {
            std::cout << "[DEBUG]   -> Case: Fireball hit. Enemy (" << this->getTextureID() << ") taking damage." << std::endl;
            takeDamage();
        }
    }
}
void Koopa :: beingCarried (Entity& other)
{
    if(Player* player = dynamic_cast<Player*>(&other))
    {
        player -> setCarriedEntity(this);
        this -> setCarried(true);
    }
}

void Koopa::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle source = animator.getCurrentFrame();
        
        float scale = 2.0f; 
        float destWidth = std::abs(source.width) * scale;
        float destHeight = std::abs(source.height) * scale;
        
        float destX = position.x;
        float destY = position.y + (spriteSize.y - destHeight);
        
        Rectangle dest = { destX, destY, destWidth, destHeight };
        
        if (facingRight) {
            source.width = -source.width;
        }
        
        DrawTexturePro(tex, source, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    } else {
        Enemy::draw();
    }
}