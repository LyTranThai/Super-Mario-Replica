#include "ExitBlock.h"
#include "Core/AssetManager.h"
#include "Core/EventSystem.h"
#include "Entities/Player.h"
#include "raylib.h"

ExitBlock::ExitBlock(Vector2 pos)
    : StaticEntity(pos, Vector2{ 32.0f, 64.0f }, Vector2{ 32.0f, 64.0f }, Vector2{ 0.0f, 0.0f }, "pipe", GREEN) {
    solid = true;
}

void ExitBlock::update(float dt) {
    (void)dt;
}

void ExitBlock::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        DrawTexturePro(tex, Rectangle{ 0, 0, (float)tex.width, (float)tex.height }, getSpriteBox(), Vector2{ 0, 0 }, 0.0f, WHITE);
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
    (void)player;
    // Interacting with exit block triggers level completion
    EventManager::getInstance().broadcast(EventType::LevelCompleted);
}
