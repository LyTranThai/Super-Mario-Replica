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

CollisionSide getOppositeSide(CollisionSide side) {
    switch (side) {
        case CollisionSide::Top: return CollisionSide::Bottom;
        case CollisionSide::Bottom: return CollisionSide::Top;
        case CollisionSide::Left: return CollisionSide::Right;
        case CollisionSide::Right: return CollisionSide::Left;
        default: return CollisionSide::None;
    }
}

void DynamicEntity::resolveOverlap(Entity& other, float overlap, CollisionSide side) {
    if (!other.isSolidFrom(side, this)) {
        // Not solid from this side, don't push out, just trigger callbacks
        this->onCollision(other, side);
        if (DynamicEntity* dynOther = dynamic_cast<DynamicEntity*>(&other)) {
            dynOther->onCollision(*this, getOppositeSide(side));
        }
        return;
    }

    // Physical push-out
    if (side == CollisionSide::Right) {
        position.x -= overlap;
        velocity.x = 0.0f;
    } else if (side == CollisionSide::Left) {
        position.x += overlap;
        velocity.x = 0.0f;
    } else if (side == CollisionSide::Bottom) {
        position.y -= overlap;
        velocity.y = 0.0f;
        setOnGround(true);
    } else if (side == CollisionSide::Top) {
        position.y += overlap;
        velocity.y = 0.0f;
    }

    // Trigger callbacks after resolving
    this->onCollision(other, side);
    if (DynamicEntity* dynOther = dynamic_cast<DynamicEntity*>(&other)) {
        dynOther->onCollision(*this, getOppositeSide(side));
    }
}
