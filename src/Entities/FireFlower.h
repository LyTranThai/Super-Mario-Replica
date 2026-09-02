#ifndef FIRE_FLOWER_H
#define FIRE_FLOWER_H

#include "Item.h"

class FireFlower : public Item {
public:
    FireFlower(Vector2 pos);
    ~FireFlower() override = default;

    void onSpawnComplete() override;
    void applyEffect(Player* player) override;
};

#endif // FIRE_FLOWER_H
