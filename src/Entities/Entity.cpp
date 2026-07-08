#include "Entity.hpp"
#include "Core/AssetManager.hpp"

Entity::Entity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor)
    : position(pos), spriteSize(sprSize), hitboxSize(hitSize), hitboxOffset(hitOffset), 
      textureID(texID), debugColor(dbgColor), active(true) {}

void Entity::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle dest = getSpriteBox();
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    } else {
        // Fallback outline/box
        DrawRectangleRec(getBoundingBox(), debugColor);
        DrawRectangleLinesEx(getBoundingBox(), 1.0f, BLACK);
    }
}

Rectangle Entity::getBoundingBox() const {
    return Rectangle{ position.x + hitboxOffset.x, position.y + hitboxOffset.y, hitboxSize.x, hitboxSize.y };
}

Rectangle Entity::getSpriteBox() const {
    return Rectangle{ position.x, position.y, spriteSize.x, spriteSize.y };
}
