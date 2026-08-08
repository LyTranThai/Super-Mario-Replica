#ifndef MOVING_STRATEGY_H
#define MOVING_STRATEGY_H

#include "raylib.h"

class MovingStrategy {
public:
    virtual ~MovingStrategy() = default;
    virtual Vector2 update(Vector2 currentPosition, float dt) = 0;
};

class HorizontalMovingStrategy final : public MovingStrategy {
private:
    float leftBound;
    float rightBound;
    float speed;
    int direction;

public:
    HorizontalMovingStrategy(float left, float right, float movementSpeed,
                             int initialDirection = 1);

    Vector2 update(Vector2 currentPosition, float dt) override;
};

class VerticalMovingStrategy final : public MovingStrategy {
private:
    float topBound;
    float bottomBound;
    float speed;
    int direction;

public:
    VerticalMovingStrategy(float top, float bottom, float movementSpeed,
                           int initialDirection = 1);

    Vector2 update(Vector2 currentPosition, float dt) override;
};

#endif // MOVING_STRATEGY_H
