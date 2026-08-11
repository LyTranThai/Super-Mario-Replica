#ifndef MOVING_PLATFORM_H
#define MOVING_PLATFORM_H

#include "Entity.h"
#include "MovingStrategy.h"
#include <memory>

class MovingPlatform final : public Entity {
private:
    std::unique_ptr<MovingStrategy> movementStrategy;
    Vector2 previousPosition;
    Vector2 frameMovement;

public:
    MovingPlatform(Vector2 pos, Vector2 size,
                   std::unique_ptr<MovingStrategy> strategy);

    void update(float dt) override;
    bool isSolid() const override { return true; }

    void setMovementStrategy(std::unique_ptr<MovingStrategy> strategy);
    Vector2 getFrameMovement() const { return frameMovement; }
    Rectangle getPreviousBoundingBox() const;
    bool wasStandingOn(Rectangle riderBounds, float tolerance = 2.0f) const;
};

#endif // MOVING_PLATFORM_H
