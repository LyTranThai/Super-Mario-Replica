#include "Star.h"
#include "Player.h"
#include <cmath>

Star::Star(Vector2 pos)
    : Item(pos, "Item", YELLOW)
{
    animator.addAnimation(AnimState::Idle, {Rectangle{508.0f, 429.0f, 66.0f, 64.0f}}, 1.0f);
    animator.setState(AnimState::Idle);
}

void Star::onSpawnComplete() {
    velocity.x = 100.0f;
    velocity.y = -200.0f; // Initial bounce
    facingRight = true;
    onGround = false;
}

void Star::updateBehavior(float dt) {
    velocity.x = facingRight ? 100.0f : -100.0f;
}

void Star::onCollision(Entity& other, CollisionSide side) {
    Item::onCollision(other, side);
    
    // Custom bounce behavior for the Star
    if (other.isSolid() && side == CollisionSide::Bottom) {
        velocity.y = -300.0f; // Bounce up when hitting the ground
        onGround = false;
    }
}

void Star::applyEffect(Player* player) {
    player->addInvincibility(5.0f);
}
