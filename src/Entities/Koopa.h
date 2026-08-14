#ifndef KOOPA_H
#define KOOPA_H

#include "Enemy.h"
#include "SpriteAnimator.h"

enum class KoopaState {
    Moving,
    Shell,
    PickedUp,
    MovingShell
};

class Koopa : public Enemy {
public:
    Koopa(Vector2 pos);
    ~Koopa() override = default;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;
    void takeDamage() override;

    bool isInShell() const { 
        return currentState == KoopaState::Shell || 
               currentState == KoopaState::PickedUp || 
               currentState == KoopaState::MovingShell; 
    }
    bool isShellMoving() const { 
        return currentState == KoopaState::MovingShell; 
    }

private:
    KoopaState currentState;
    SpriteAnimator animator;
};

#endif // KOOPA_H
