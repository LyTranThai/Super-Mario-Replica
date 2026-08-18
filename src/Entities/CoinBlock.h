#ifndef COIN_BLOCK_H
#define COIN_BLOCK_H

#include "InteractiveBlock.h"

class CoinBlock : public InteractiveBlock {
private:
    int coinAmount; // Số lượng đồng xu trong khối (thường là 1 hoặc nhiều xu)

public:
    CoinBlock(Vector2 pos, int coins = 1);
    ~CoinBlock() override = default;

    void hit(Player& player) override;
    void spawnCoinAnimation();
};

#endif // COIN_BLOCK_H