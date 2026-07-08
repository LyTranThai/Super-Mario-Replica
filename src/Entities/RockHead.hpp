#ifndef ROCK_HEAD_H
#define ROCK_HEAD_H

#include "Enemy.hpp"

enum class RockState {
    Idle,
    Slamming,
    Waiting,
    Rising
};

class RockHead : public Enemy {
private:
    RockState rockState;
    float timer;
    Vector2 spawnPosition;

public:
    RockHead(Vector2 pos);
    ~RockHead() override = default;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;
    void checkTrigger(Vector2 playerPos);
};

#endif // ROCK_HEAD_H
