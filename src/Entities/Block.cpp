#include "Block.hpp"

Block::Block(Vector2 pos, const std::string& texID, Color dbgColor)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, texID, dbgColor) {}

void Block::update(float dt) {
    (void)dt;
}

void Block::onInteract(Player& player) {
    (void)player;
}
