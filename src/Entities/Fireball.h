#ifndef FIREBALL_H
#define FIREBALL_H

#include "DynamicEntity.h"

struct FireballSpawnData {
    Vector2 position;
    bool facingRight;
};

class Fireball : public DynamicEntity {
private:
    float bounceForce;
    float speed;
    int bouncesLeft;
    float animTimer;
    int currentFrame;

public:
    Fireball(Vector2 pos, bool faceRight);
    ~Fireball() override = default;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;
    void explode();

    Rectangle getCurrentFrame() const;
};

#endif // FIREBALL_H
