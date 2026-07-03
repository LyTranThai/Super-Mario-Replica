#ifndef KOOPA_H
#define KOOPA_H

#include "Enemy.h"

class Koopa : public Enemy {
private:
    bool inShell;
    bool shellMoving;

public:
    Koopa(Vector2 pos);
    ~Koopa() override = default;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;
    void takeDamage() override;
    
    bool isInShell() const { return inShell; }
    bool isShellMoving() const { return shellMoving; }
};

#endif // KOOPA_H
