#include "Goomba.h"
#include "SpriteAnimator.h"
#include <memory>

Goomba::Goomba(Vector2 pos) 
    : Enemy(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 24.0f, 24.0f }, Vector2{ 4.0f, 8.0f }, "goomba", RED) {
    
    animator = std::make_unique<SpriteAnimator>();
    SpriteAnimator* sprAnim = static_cast<SpriteAnimator*>(animator.get());
    
    // Setup animations based on estimated spritesheet coordinates
    sprAnim->addAnimation("walk", { Rectangle{0, 4, 16, 16}, Rectangle{30, 4, 16, 16} }, 0.15f);
    sprAnim->addAnimation("die", { Rectangle{60, 8, 16, 8} }, 1.0f, false);
    
    sprAnim->setState("walk");
}
