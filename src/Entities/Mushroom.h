#ifndef MUSHROOM_H
#define MUSHROOM_H

#include "DynamicEntity.h"
#include "SpriteAnimator.h"

class Mushroom : public DynamicEntity {
private:
    float spawnRiseTimer;
    Vector2 targetSpawnPosition;
    SpriteAnimator animator;

public:
    Mushroom(Vector2 pos);
    ~Mushroom() override = default;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;
};

#endif // MUSHROOM_H
