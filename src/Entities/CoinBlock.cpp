#include "CoinBlock.h"

CoinBlock::CoinBlock(Vector2 pos, int coins)
    : InteractiveBlock(pos, InteractiveBlockType::Question, ItemType::Coin), coinAmount(coins) {}

void CoinBlock::hit(Player& player) {
    if (isUsedUp()) return;

    if (coinAmount > 0) {
        coinAmount--;
        
        // Cộng điểm và coin cho Mario
        // player.addCoin(1);
        // player.addScore(200);

        spawnCoinAnimation(); // Hiện hiệu ứng đồng xu nảy lên
        
        if (coinAmount == 0) {
            // Đánh dấu khối đã dùng hết -> đổi sprite sang khối xám rỗng (Empty/Used Block)
            InteractiveBlock::hit(player); 
        }
    }
}

void CoinBlock::spawnCoinAnimation() {
    // TODO: Tạo một hiệu ứng Coin nảy lên từ vị trí block rồi biến mất
}