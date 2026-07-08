#ifndef ENEMY_H
#define ENEMY_H

#include "DynamicEntity.hpp"

class AIBehaviorStrategy {
public:
    virtual ~AIBehaviorStrategy() = default;
    virtual void updateAI(DynamicEntity& enemy, float dt) = 0;
};

// Standard patrol strategy for Goombas and Koopas
class PatrolHorizontalStrategy : public AIBehaviorStrategy {
private:
    float speed;
public:
    PatrolHorizontalStrategy(float spd = 60.0f) : speed(spd) {}
    void updateAI(DynamicEntity& enemy, float dt) override {
        (void)dt;
        Vector2 vel = enemy.getVelocity();
        // If speed is not set, set it
        if (vel.x == 0.0f) {
            vel.x = enemy.isFacingRight() ? speed : -speed;
        }
        enemy.setVelocity(vel);
    }
};

class Enemy : public DynamicEntity {
protected:
    AIBehaviorStrategy* aiStrategy;

public:
    Enemy(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor = RED);
    virtual ~Enemy() override;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;
    virtual void takeDamage();
};

#endif // ENEMY_H
