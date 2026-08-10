#include "Entity.h"
#include "Core/AssetManager.h"
#include <cmath>

Entity::Entity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor)
    : position(pos), spriteSize(sprSize), hitboxSize(hitSize), hitboxOffset(hitOffset), 
      textureID(texID), debugColor(dbgColor), active(true) {}

void Entity::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle source;
        if (animator) {
            source = animator->getCurrentFrame();
        } else {
            source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        }
        
        float srcWidth = std::abs(source.width);
        float srcHeight = std::abs(source.height);
        
        // Calculate scale to match spriteSize height
        float scale = spriteSize.y / srcHeight;
        float destWidth = srcWidth * scale;
        float destHeight = spriteSize.y;
        
        // Center the sprite horizontally on the hitbox
        float hitboxCenter = position.x + hitboxOffset.x + hitboxSize.x / 2.0f;
        float destX = hitboxCenter - destWidth / 2.0f;
        float destY = position.y; // Standard top alignment
        
        Rectangle dest = { destX, destY, destWidth, destHeight };

        // Flip texture if facing left (assuming base sprite sheet faces right)
        // Note: the master enemy spritesheet faces LEFT, so we flip when facing RIGHT.
        // Wait, Goombas are symmetrical. Koopas in the sheet face LEFT.
        // Let's assume standard behavior: if isFacingRight() is true, and sheet faces left, we flip.
        if (isFacingRight()) {
            source.width = -source.width;
        }

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
