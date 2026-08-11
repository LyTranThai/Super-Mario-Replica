#include "MovingPlatform.h"
#include <cmath>
#include <utility>

MovingPlatform::MovingPlatform(Vector2 pos, Vector2 size,
                               std::unique_ptr<MovingStrategy> strategy)
    : Entity(pos, size, size, Vector2{0.0f, 0.0f}, "solid", DARKBROWN),
      movementStrategy(std::move(strategy)),
      previousPosition(pos),
      frameMovement{0.0f, 0.0f} {}

void MovingPlatform::update(float dt) {
    previousPosition = position;

    if (movementStrategy) {
        position = movementStrategy->update(position, dt);
    }

    frameMovement = Vector2{
        position.x - previousPosition.x,
        position.y - previousPosition.y
    };
}

void MovingPlatform::setMovementStrategy(
    std::unique_ptr<MovingStrategy> strategy) {
    movementStrategy = std::move(strategy);
}

Rectangle MovingPlatform::getPreviousBoundingBox() const {
    return Rectangle{
        previousPosition.x + hitboxOffset.x,
        previousPosition.y + hitboxOffset.y,
        hitboxSize.x,
        hitboxSize.y
    };
}

bool MovingPlatform::wasStandingOn(Rectangle riderBounds,
                                   float tolerance) const {
    Rectangle previousBounds = getPreviousBoundingBox();
    float riderBottom = riderBounds.y + riderBounds.height;

    bool horizontallyOverlapping =
        riderBounds.x + riderBounds.width > previousBounds.x &&
        riderBounds.x < previousBounds.x + previousBounds.width;
    bool touchingTop =
        std::abs(riderBottom - previousBounds.y) <= tolerance;

    return horizontallyOverlapping && touchingTop &&
           riderBounds.y < previousBounds.y;
}
