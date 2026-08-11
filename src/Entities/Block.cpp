#include "Block.h"
#include "Core/AssetManager.h"

Block::Block(Vector2 pos, Type t, bool top, const std::string& texID, Color dbgColor)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, texID, dbgColor), blockType(t), isTopGround(top) {}

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
            source = { 2192.0f, 144.0f, 16.0f, 16.0f };
        } else if (blockType == Type::FlyingBrick) {
            source = { 320.0f, 144.0f, 16.0f, 16.0f };
        } else {
            int col = (int)(position.x / 32.0f);
            float srcX = (float)((col * 16) % 1104);
            // Grass on top (208 to 239, h=31), dirt on bottom (221 to 239, h=18)
            float srcY = isTopGround ? 208.0f : 221.0f;
            float srcH = isTopGround ? 31.0f : 18.0f;
            source = { srcX, srcY, 16.0f, srcH };
        }
        
        Rectangle dest = getSpriteBox();
        if (blockType == Type::Ground) {
            dest.height = source.height * 2.0f; // Scale height by 2.0x just like width
        }
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(worldTex, source, dest, origin, 0.0f, WHITE);
    } else {
        Entity::draw();
    }
}
