#ifndef ITEM_H
#define ITEM_H

#include "DynamicEntity.h"
#include "InteractiveBlock.h"
#include "SpriteAnimator.h"

class Player;

class Item : public DynamicEntity {
protected:
    float spawnRiseTimer;
    Vector2 targetSpawnPosition;
    float riseSpeed;
    SpriteAnimator animator;

public:
    Item(Vector2 pos, const std::string& texID, Color dbgColor);
    virtual ~Item() override = default;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;

    virtual void onSpawnComplete() {}
    virtual void updateBehavior(float dt) {}
    virtual void applyEffect(Player* player) = 0;
};

#endif // ITEM_H
