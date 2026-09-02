#include "InteractiveBlock.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include "Core/AssetManager.h"
#include <iostream>
#include <cmath>

InteractiveBlock::InteractiveBlock(Vector2 pos, InteractiveBlockType type)
    : StaticEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 32.0f, 32.0f }, Vector2{ 0.0f, 0.0f }, 
                   (type == InteractiveBlockType::Question) ? "question" : "brick", 
                   (type == InteractiveBlockType::Question) ? GOLD : ORANGE),
      blockType(type), isUsed(false), bounceTimer(0.0f), originalPosition(pos) {}

void InteractiveBlock::update(float dt) {
    
    if (bounceTimer > 0.0f) {
        bounceTimer -= dt;
        
        // Simple cosine offset for visual bounce animation
        float offset = sinf((bounceTimer / 0.15f) * PI) * 8.0f;
        position.y = originalPosition.y - offset;

        if (bounceTimer <= 0.0f) {
            position.y = originalPosition.y;
        }
    }
}

void InteractiveBlock::draw() {
    if (blockType == InteractiveBlockType::Question && isUsed) {
        // Draw the hit empty question block texture
        Texture2D tex = AssetManager::getInstance().getTexture("world11");
        if (tex.id != 0) {
            Rectangle source = { 3008.0f, 80.0f, 16.0f, 16.0f }; // (3008,80) -> (3023,96)
            Rectangle dest = getSpriteBox();
            DrawTexturePro(tex, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            DrawRectangleRec(getBoundingBox(), GRAY);
            DrawRectangleLinesEx(getBoundingBox(), 1.0f, BLACK);
        }
        return;
    }

    Texture2D worldTex = AssetManager::getInstance().getTexture("world");
    if (worldTex.id != 0) {
        Rectangle source;
        if (blockType == InteractiveBlockType::Question) {
            source = { 256.0f, 144.0f, 16.0f, 16.0f };
        } else {
            source = { 320.0f, 144.0f, 16.0f, 16.0f };
        }
        Rectangle dest = getSpriteBox();
        DrawTexturePro(worldTex, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        // Fallback outline/box
        DrawRectangleRec(getBoundingBox(), debugColor);
        DrawRectangleLinesEx(getBoundingBox(), 1.0f, BLACK);
    }
}

void InteractiveBlock::onInteract(Player& player) {
    hit(player);
}
