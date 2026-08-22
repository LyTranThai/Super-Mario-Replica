#include "ExitBlock.h"
#include "Core/AssetManager.h"
#include "Core/EventSystem.h"
#include "Entities/Player.h"
#include "raylib.h"

ExitBlock::ExitBlock(Vector2 pos)
    : StaticEntity(pos, Vector2{ 64.0f, 64.0f }, Vector2{ 64.0f, 64.0f }, Vector2{ 0.0f, 0.0f }, "world", GREEN) {
    solid = true;
}

void ExitBlock::update(float dt) {
    (void)dt;
}

void ExitBlock::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture("world11");
    if (tex.id != 0) {
        Rectangle src = { 448.0f, 176.0f, 32.0f, 32.0f }; // Short pipe from world11 texture
        DrawTexturePro(tex, src, getSpriteBox(), Vector2{ 0, 0 }, 0.0f, WHITE);
    } else {
        // Render fallback Exit Warp Pipe / Door
        Rectangle rect = getSpriteBox();
        DrawRectangleRec(rect, Color{ 30, 180, 80, 255 }); // Dark Green pipe body
        DrawRectangleLinesEx(rect, 2.0f, DARKGREEN);
        
        // Pipe Top Rim
        DrawRectangle((int)rect.x - 2, (int)rect.y, (int)rect.width + 4, 16, Color{ 40, 220, 100, 255 });
        DrawRectangleLinesEx(Rectangle{ rect.x - 2, rect.y, rect.width + 4, 16 }, 2.0f, DARKGREEN);

        // "EXIT" indicator text
        DrawText("EXIT", (int)rect.x - 2, (int)rect.y - 18, 14, GOLD);
    }
}

void ExitBlock::onInteract(Player& player) {
    // Only interact if player presses down on top of it, but this is handled by Level::update now
}
