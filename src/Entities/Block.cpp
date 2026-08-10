#include "Block.h"
#include "Core/AssetManager.h"

Block::Block(Vector2 pos, Type t, const std::string& texID, Color dbgColor)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, texID, dbgColor), blockType(t) {}

void Block::update(float dt) {
    (void)dt;
}

void Block::onInteract(Player& player) {
    (void)player;
}

void Block::draw() {
    Texture2D worldTex = AssetManager::getInstance().getTexture("world");
    if (worldTex.id != 0) {
        Rectangle source;
        if (blockType == Type::Brick) {
            // Normal brick that stacks
            source = { 2192.0f, 144.0f, 16.0f, 16.0f };
        } else {
            // Ground floor slice (0,208) -> (1103, 239)
            int col = (int)(position.x / 32.0f);
            float srcX = (float)((col * 16) % 1104);
            // Use top half (208) for simplicity, or we could alternate based on Y. Let's use 208.
            float srcY = 208.0f; 
            source = { srcX, srcY, 16.0f, 16.0f };
        }
        
        Rectangle dest = getSpriteBox();
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(worldTex, source, dest, origin, 0.0f, WHITE);
    } else {
        Entity::draw();
    }
}
