#ifndef HEART_H
#define HEART_H

#include "Item.h"

class Heart : public Item {
public:
    Heart(Vector2 pos);
    ~Heart() override = default;

    void onSpawnComplete() override;
    void applyEffect(Player* player) override;
};

#endif // HEART_H
