#include "Block.h"
#include "SpriteAnimator.h"

Block::Block(Vector2 pos, const std::string& texID, Color dbgColor)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, texID, dbgColor) {
    animator = std::make_unique<SpriteAnimator>();
    static_cast<SpriteAnimator*>(animator.get())->addAnimation("idle", {{0, 0, 16, 16}}, 1.0f);
    static_cast<SpriteAnimator*>(animator.get())->setState("idle");
}

void Block::update(float dt) {
    if (animator) animator->update(dt);
}

void Block::onInteract(Player& player) {
    (void)player;
}
