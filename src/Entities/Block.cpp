#include "Block.h"
#include "Core/AssetManager.h"

Block::Block(Vector2 pos, const std::string& texID, Color dbgColor)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, texID, dbgColor) {}

void Block::update(float dt) {
    (void)dt;
}

void Block::onInteract(Player& player) {
    (void)player;
}

void Block::draw() {
    Texture2D worldTex = AssetManager::getInstance().getTexture("world");
    if (worldTex.id != 0) {
        // Platform below sprite: (0, 208) -> (15, 223)
        Rectangle source = { 0.0f, 208.0f, 16.0f, 16.0f };
        Rectangle dest = getSpriteBox();
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(worldTex, source, dest, origin, 0.0f, WHITE);
    } else {
        Entity::draw();
    }
}
