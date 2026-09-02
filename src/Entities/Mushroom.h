#ifndef MUSHROOM_H
#define MUSHROOM_H

#include "Item.h"

class Mushroom : public Item {
public:
    Mushroom(Vector2 pos);
    ~Mushroom() override = default;

    void onSpawnComplete() override;
    void updateBehavior(float dt) override;
    void applyEffect(Player* player) override;
};

#endif // MUSHROOM_H
