#include "TeleportPipe.h"
#include "Core/AssetManager.h"
#include "raylib.h"

TeleportPipe::TeleportPipe(Vector2 pos)
    : StaticEntity(pos, Vector2{ 64.0f, 64.0f }, Vector2{ 64.0f, 64.0f }, Vector2{ 0.0f, 0.0f }, "world", BLUE) {
    solid = true;
}

void TeleportPipe::update(float dt) {
    (void)dt;
}

void TeleportPipe::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle src = { 736.0f, 144.0f, 32.0f, 65.0f }; // Medium pipe from world texture
        DrawTexturePro(tex, src, getSpriteBox(), Vector2{ 0, 0 }, 0.0f, WHITE);
    } else {
        Rectangle rect = getSpriteBox();
        DrawRectangleRec(rect, Color{ 30, 80, 180, 255 }); // Blueish fallback pipe body
        DrawRectangleLinesEx(rect, 2.0f, DARKBLUE);
        
        DrawRectangle((int)rect.x - 2, (int)rect.y, (int)rect.width + 4, 16, Color{ 40, 100, 220, 255 });
        DrawRectangleLinesEx(Rectangle{ rect.x - 2, rect.y, rect.width + 4, 16 }, 2.0f, DARKBLUE);
    }
}

void TeleportPipe::onInteract(Player& player) {
    // Handled in Level::update
}
