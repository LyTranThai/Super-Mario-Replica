#ifndef STAR_H
#define STAR_H

#include "Item.h"

class Star : public Item {
public:
    Star(Vector2 pos);
    ~Star() override = default;

    void onSpawnComplete() override;
    void updateBehavior(float dt) override;
    void applyEffect(Player* player) override;
    void onCollision(Entity& other, CollisionSide side) override;
};

#endif // STAR_H
