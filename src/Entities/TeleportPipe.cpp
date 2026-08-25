#include "TeleportPipe.h"
#include "Core/AssetManager.h"
#include "raylib.h"

static float getPipeHeight(PipeSize size) {
    if (size == PipeSize::Short) return 64.0f;
    if (size == PipeSize::Medium) return 96.0f;
    return 128.0f; // Long
}

TeleportPipe::TeleportPipe(Vector2 pos, PipeSize size)
    : StaticEntity(Vector2{pos.x, pos.y - (getPipeHeight(size) - 32.0f)}, 
                   Vector2{ 64.0f, getPipeHeight(size) }, 
                   Vector2{ 64.0f, getPipeHeight(size) }, 
                   Vector2{ 0.0f, 0.0f }, "world11", BLUE), pipeSize(size) {
    solid = true;
}

void TeleportPipe::update(float dt) {
    (void)dt;
}

void TeleportPipe::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle src;
        if (pipeSize == PipeSize::Short) src = { 448.0f, 175.0f, 32.0f, 34.0f }; // (448,175) -> (479,208)
        else if (pipeSize == PipeSize::Medium) src = { 608.0f, 160.0f, 32.0f, 49.0f }; // (608,160) -> (639,208)
        else src = { 736.0f, 144.0f, 32.0f, 65.0f }; // (736,144) -> (767,208)

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
