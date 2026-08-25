#include "PiranhaPlant.h"
#include "Player.h"
#include "Core/AssetManager.h"
#include <cmath>
#include <iostream>

PiranhaPlant::PiranhaPlant(Vector2 pos)
    : Enemy(pos, Vector2{ 32.0f, 48.0f }, Vector2{ 24.0f, 40.0f }, Vector2{ 4.0f, 8.0f }, "enemy", RED),
      plantState(PlantState::Hidden), timer(2.0f), pipePosition(pos), currentYOffset(0.0f) {
    // Delete base patrol strategy (Piranha Plants are anchored in space)
    delete aiStrategy;
    aiStrategy = nullptr;
    velocity = Vector2{ 0.0f, 0.0f };
    onGround = true; // Prevents gravity calculations

    animator.addAnimation(AnimState::Idle, {
        Rectangle{ 239.0f, 37.0f, 16.0f, 24.0f },
        Rectangle{ 256.0f, 37.0f, 16.0f, 24.0f }
    }, 0.2f);
    animator.setState(AnimState::Idle);
}

void PiranhaPlant::update(float dt) {
    timer -= dt;
    switch (plantState) {
        case PlantState::Hidden:
            currentYOffset = 0.0f;
            if (timer <= 0.0f) {
                plantState = PlantState::Emerging;
                timer = 1.5f;
            }
            break;
        case PlantState::Emerging:
            currentYOffset = ((1.5f - timer) / 1.5f) * maxOffset;
            if (timer <= 0.0f) {
                plantState = PlantState::Extended;
                timer = 2.0f;
                currentYOffset = maxOffset;
            }
            break;
        case PlantState::Extended:
            currentYOffset = maxOffset;
            if (timer <= 0.0f) {
                plantState = PlantState::Retracting;
                timer = 1.5f;
            }
            break;
        case PlantState::Retracting:
            currentYOffset = (timer / 1.5f) * maxOffset;
            if (timer <= 0.0f) {
                plantState = PlantState::Hidden;
                timer = 2.0f;
                currentYOffset = 0.0f;
            }
            break;
    }
    // Update vertical coordinates
    position.y = pipePosition.y - currentYOffset;

    animator.update(dt);
}

void PiranhaPlant::onCollision(Entity& other, CollisionSide side) {
    (void)side;
    if (!other.isActive()) return;

    
    Player* player = dynamic_cast<Player*>(&other);
    if (player) {
        std::cout << "[DEBUG]   -> Case: Player touched Piranha Plant. Player taking damage." << std::endl;
        player->takeDamage();
    }
}

void PiranhaPlant::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle source = animator.getCurrentFrame();
        
        float scale = 2.0f; // Scale 16x24 sprite to 32x48 size
        float destWidth = std::abs(source.width) * scale;
        float destHeight = std::abs(source.height) * scale;
        
        float destX = position.x;
        float destY = position.y + (48.0f - destHeight); // Align to bottom
        
        Rectangle dest = { destX, destY, destWidth, destHeight };
        
        if (facingRight) {
            source.width = -source.width;
        }
        
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    } else {
        Enemy::draw();
    }
}
