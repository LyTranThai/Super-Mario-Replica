#ifndef VISUAL_COIN_H
#define VISUAL_COIN_H

#include "Item.h"

class VisualCoin : public Item {
public:
    VisualCoin(Vector2 pos);
    ~VisualCoin() override = default;

    void onSpawnComplete() override;
    void applyEffect(Player* player) override;
};

#endif // VISUAL_COIN_H
