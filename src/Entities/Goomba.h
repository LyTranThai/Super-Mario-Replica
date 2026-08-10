#ifndef GOOMBA_H
#define GOOMBA_H

#include "Enemy.h"
#include "SpriteAnimator.h"

class Goomba : public Enemy {
private:
    float deathTimer;
    bool isDead;
public:
    Goomba(Vector2 pos);
    ~Goomba() override = default;

    void update(float dt) override;
    void draw() override;
    void takeDamage() override;
};

#endif // GOOMBA_H
