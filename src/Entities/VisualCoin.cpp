#include "VisualCoin.h"

VisualCoin::VisualCoin(Vector2 pos)
    : Item(pos, "coin", YELLOW)
{
    animator.addAnimation(AnimState::Idle, {Rectangle{243.0f, 66.0f, 10.0f, 15.0f}}, 1.0f);
    animator.setState(AnimState::Idle);
    
    // Coin pops up higher and faster
    targetSpawnPosition.y = pos.y - 64.0f;
    spawnRiseTimer = 0.3f;
    riseSpeed = 64.0f / 0.3f;
}

void VisualCoin::onSpawnComplete() {
    // Coin disappears immediately after popping out
    active = false;
}

void VisualCoin::applyEffect(Player* player) {
    // Does nothing. Actual coin logic is handled by the block that spawned it.
}
