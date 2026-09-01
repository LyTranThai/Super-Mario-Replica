#ifndef FIREBALL_H
#define FIREBALL_H

#include "DynamicEntity.h"

enum class FireballType {
    Fireball1,
    Fireball2
};

struct FireballSpawnData {
    Vector2 position;
    bool facingRight;
    FireballType type = FireballType::Fireball1;
};

class Fireball : public DynamicEntity {
private:
    FireballType fireballType;
    float bounceForce;
    float speed;
    int bouncesLeft;
    float animTimer;
    int currentFrame;
    float stateTimer; // For Fireball2 state duration tracking
    int state;        // 1, 2, 3 for Fireball2

public:
    Fireball(Vector2 pos, bool faceRight, FireballType type = FireballType::Fireball1);
    ~Fireball() override = default;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;
    void explode();

    Rectangle getCurrentFrame() const;
    FireballType getType() const { return fireballType; }
};

#endif // FIREBALL_H
