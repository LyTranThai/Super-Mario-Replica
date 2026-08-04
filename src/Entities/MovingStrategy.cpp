#include "MovingStrategy.h"
#include <algorithm>
#include <cmath>

namespace {
float moveBetweenBounds(float value, float lowerBound, float upperBound,
                        float distance, int& direction) {
    if (lowerBound == upperBound) {
        return lowerBound;
    }

    float nextValue = value + distance * static_cast<float>(direction);

    // Reflect any overshoot so movement remains smooth near either endpoint.
    while (nextValue < lowerBound || nextValue > upperBound) {
        if (nextValue > upperBound) {
            nextValue = upperBound - (nextValue - upperBound);
            direction = -1;
        } else if (nextValue < lowerBound) {
            nextValue = lowerBound + (lowerBound - nextValue);
            direction = 1;
        }
    }

    return nextValue;
}
} // namespace

HorizontalMovingStrategy::HorizontalMovingStrategy(
    float left, float right, float movementSpeed, int initialDirection)
    : leftBound(std::min(left, right)),
      rightBound(std::max(left, right)),
      speed(std::abs(movementSpeed)),
      direction(initialDirection < 0 ? -1 : 1) {}

Vector2 HorizontalMovingStrategy::update(Vector2 currentPosition, float dt) {
    if (dt <= 0.0f || speed == 0.0f) {
        return currentPosition;
    }

    currentPosition.x = moveBetweenBounds(
        currentPosition.x, leftBound, rightBound, speed * dt, direction);
    return currentPosition;
}

VerticalMovingStrategy::VerticalMovingStrategy(
    float top, float bottom, float movementSpeed, int initialDirection)
    : topBound(std::min(top, bottom)),
      bottomBound(std::max(top, bottom)),
      speed(std::abs(movementSpeed)),
      direction(initialDirection < 0 ? -1 : 1) {}

Vector2 VerticalMovingStrategy::update(Vector2 currentPosition, float dt) {
    if (dt <= 0.0f || speed == 0.0f) {
        return currentPosition;
    }

    currentPosition.y = moveBetweenBounds(
        currentPosition.y, topBound, bottomBound, speed * dt, direction);
    return currentPosition;
}
