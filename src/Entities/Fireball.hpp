#ifndef FIREBALL_H
#define FIREBALL_H

#include "DynamicEntity.hpp"

class Fireball : public DynamicEntity {
private:
    float bounceForce;
    float speed;
    int bouncesLeft;

public:
    Fireball(Vector2 pos, bool faceRight);
    ~Fireball() override = default;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;
    void explode();
};

#endif // FIREBALL_H
