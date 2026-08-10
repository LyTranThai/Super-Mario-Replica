#include "PiranhaPlant.h"
#include "Player.h"
#include "SpriteAnimator.h"
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

    animator = std::make_unique<SpriteAnimator>();
    static_cast<SpriteAnimator*>(animator.get())->addAnimation("bite", {{192, 0, 16, 24}, {208, 0, 16, 24}}, 6.0f);
    static_cast<SpriteAnimator*>(animator.get())->setState("bite");
}

void PiranhaPlant::update(float dt) {
    if (animator) animator->update(dt);
    
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
