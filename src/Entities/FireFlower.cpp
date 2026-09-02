#include "FireFlower.h"
#include "Player.h"
#include "Core/EventSystem.h"

FireFlower::FireFlower(Vector2 pos)
    : Item(pos, "Item", ORANGE)
{
    animator.addAnimation(AnimState::Idle, {Rectangle{220.0f, 213.0f, 70.0f, 64.0f}}, 1.0f);
    animator.setState(AnimState::Idle);
}

void FireFlower::onSpawnComplete() {
    velocity.x = 100.0f;
    // Let gravity pull it back down to the block after the hop
}

void FireFlower::applyEffect(Player* player) {
    player->powerUp(PowerStateType::Fire);
    EventManager::getInstance().broadcast(EventType::PowerUpCollected);
}
