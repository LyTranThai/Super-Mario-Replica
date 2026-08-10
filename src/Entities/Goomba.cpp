#include "Goomba.h"
#include "SpriteAnimator.h"
#include <memory>
#include "Core/AssetManager.h"
#include "Core/EventSystem.h"
#include <iostream>

Goomba::Goomba(Vector2 pos) 
    : Enemy(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 24.0f, 24.0f }, Vector2{ 4.0f, 8.0f }, "enemy", RED),
      deathTimer(0.0f), isDead(false) {
    
    animator = std::make_unique<SpriteAnimator>();
    SpriteAnimator* sprAnim = static_cast<SpriteAnimator*>(animator.get());
    
    sprAnim->addAnimation("walk", {
        Rectangle{ 0.0f, 16.0f, 16.0f, 16.0f },
        Rectangle{ 18.0f, 16.0f, 16.0f, 16.0f }
    }, 0.2f);
    
    sprAnim->addAnimation("die", {
        Rectangle{ 36.0f, 24.0f, 16.0f, 8.0f }
    }, 1.0f, false);
    
    sprAnim->setState("walk");
}

void Goomba::update(float dt) {
    if (isDead) {
        deathTimer -= dt;
        if (deathTimer <= 0.0f) {
            active = false;
        }
        return; // Don't move while dead
    }
    
    Enemy::update(dt);
    if (animator) animator->update(dt);
}

void Goomba::takeDamage() {
    if (isDead) return;
    
    std::cout << "[DEBUG] Goomba was crushed." << std::endl;
    isDead = true;
    deathTimer = 0.5f; // linger on screen
    if (animator) static_cast<SpriteAnimator*>(animator.get())->setState("die");
    velocity = Vector2{ 0.0f, 0.0f }; // stop moving
    EventManager::getInstance().broadcast(EventType::EnemyStomped);
}

void Goomba::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0 && animator) {
        Rectangle source = static_cast<SpriteAnimator*>(animator.get())->getCurrentFrame();
        
        float scale = 2.0f; // Scale 16x16 sprite to 32x32 size
        float destWidth = std::abs(source.width) * scale;
        float destHeight = std::abs(source.height) * scale;
        
        float destX = position.x;
        float destY = position.y + (32.0f - destHeight); // Align to bottom
        
        Rectangle dest = { destX, destY, destWidth, destHeight };
        
        if (!facingRight) {
            source.width = -source.width;
        }
        
        DrawTexturePro(tex, source, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    } else {
        Enemy::draw();
    }
}
