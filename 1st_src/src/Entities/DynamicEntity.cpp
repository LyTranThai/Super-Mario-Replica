#include "DynamicEntity.h"

DynamicEntity::DynamicEntity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor)
    : Entity(pos, sprSize, hitSize, hitOffset, texID, dbgColor), 
      velocity{0.0f, 0.0f}, acceleration{0.0f, 0.0f}, onGround(false), facingRight(true) {}

void DynamicEntity::applyGravity(float dt) {
    if (!onGround) {
        velocity.y += gravity * dt;
        if (velocity.y > terminalVelocity) {
            velocity.y = terminalVelocity;
        }
    }
}
