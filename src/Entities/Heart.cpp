#include "Heart.h"
#include "Player.h"

Heart::Heart(Vector2 pos)
    : Item(pos, "Item", PINK)
{
    animator.addAnimation(AnimState::Idle, {Rectangle{76.0f, 61.0f, 66.0f, 60.0f}}, 1.0f);
    animator.setState(AnimState::Idle);
}

void Heart::onSpawnComplete() {
    velocity.x = 0.0f;
    // Don't set onGround = true, let gravity pull it back down to the block after the hop
}

void Heart::applyEffect(Player* player) {
    player->addLives(1);
    // You could also broadcast a 1UP event here if you have one
}
