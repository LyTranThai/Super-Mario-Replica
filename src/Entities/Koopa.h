#ifndef KOOPA_H
#define KOOPA_H

#include "Enemy.h"

class Koopa : public Enemy {
private:
    bool inShell;
    bool shellMoving;
    bool carried;

public:
    Koopa(Vector2 pos);
    ~Koopa() override = default;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;
    void takeDamage() override;
    
    bool isInShell() const { return inShell; }
    bool isShellMoving() const { return shellMoving; }
    void setShellMoving(bool state) { shellMoving = state; }
    
    void setCarried(bool state) { carried = state; }
    bool isCarried() const { return carried; }
    void beingCarried(Entity& other);
};

#endif // KOOPA_H
