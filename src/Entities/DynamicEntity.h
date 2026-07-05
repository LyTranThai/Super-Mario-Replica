#ifndef DYNAMIC_ENTITY_H
#define DYNAMIC_ENTITY_H

#include "Entity.h"

enum class CollisionSide {
    None,
    Top,
    Bottom,
    Left,
    Right
};

class DynamicEntity : public Entity {
protected:
    Vector2 velocity;
    Vector2 acceleration;
    bool onGround;
    bool facingRight;

    // Physics parameters constants
    static constexpr float gravity = 800.0f;
    static constexpr float terminalVelocity = 500.0f;

public:
    DynamicEntity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor = RED);
    virtual ~DynamicEntity() override = default;

    virtual void onCollision(Entity& other, CollisionSide side) = 0;
    
    void applyGravity(float dt);

    Vector2 getVelocity() const { return velocity; }
    void setVelocity(Vector2 vel) { velocity = vel; }

    Vector2 getAcceleration() const { return acceleration; }
    void setAcceleration(Vector2 acc) { acceleration = acc; }

    bool isOnGround() const { return onGround; }
    virtual void setOnGround(bool state) { onGround = state; }

    bool isFacingRight() const { return facingRight; }
    void setFacingRight(bool state) { facingRight = state; }
};

#endif // DYNAMIC_ENTITY_H
