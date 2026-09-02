#include "Mushroom.h"
#include "Player.h"
#include "Core/EventSystem.h"

Mushroom::Mushroom(Vector2 pos)
    : Item(pos, "Item", RED)
{
    animator.addAnimation(AnimState::Idle, {Rectangle{220.0f, 357.0f, 66.0f, 64.0f}}, 1.0f);
    //{Rectangle{232.0f, 357.0f, 64.0f, 66.0f}}, 1.0f);
    animator.setState(AnimState::Idle);
}

void Mushroom::onSpawnComplete() {
    velocity.x = 80.0f;
    facingRight = true;
}

void Mushroom::updateBehavior(float dt) {
    velocity.x = facingRight ? 80.0f : -80.0f;
}

void Mushroom::applyEffect(Player* player) {
    player->powerUp(PowerStateType::Super);
    EventManager::getInstance().broadcast(EventType::PowerUpCollected);
}
