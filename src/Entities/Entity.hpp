#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include <string>

class Entity {
protected:
    Vector2 position;      // Top-left visual coordinate
    Vector2 spriteSize;    // Dimensions for texture drawing
    Vector2 hitboxSize;    // Dimensions for physical collision box
    Vector2 hitboxOffset;  // Offset positioning hitbox relative to position
    std::string textureID; // Asset manager cache key
    Color debugColor;      // Fallback debug render color
    bool active;           // Lifecycle flag

public:
    Entity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor = RED);
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void draw(); // Default draws texture or debug box fallback

    Rectangle getBoundingBox() const; // For physics collision checking
    virtual Rectangle getSpriteBox() const;   // For texture rendering

    Vector2 getPosition() const { return position; }
    void setPosition(Vector2 pos) { position = pos; }

    Vector2 getSpriteSize() const { return spriteSize; }
    Vector2 getHitboxSize() const { return hitboxSize; }

    bool isActive() const { return active; }
    void setActive(bool state) { active = state; }

    virtual bool isSolid() const { return false; } // Handled dynamically
    virtual bool isCarried() const { return false; }
};

#endif // ENTITY_H
