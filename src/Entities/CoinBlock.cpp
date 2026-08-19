#include "CoinBlock.h"
#include "Core/EventSystem.h"
#include "Player.h"

struct ItemSpawnData {
    Vector2 position;
    ItemType type;
};

CoinBlock::CoinBlock(Vector2 pos, int coins)
    : InteractiveBlock(pos, InteractiveBlockType::Question, ItemType::Coin), coinAmount(coins) {}

void CoinBlock::hit(Player& player) {
    if (isUsedUp()) return;

    if (coinAmount > 0) {
        coinAmount--;
        
        // Cộng điểm và coin cho Mario
        player.addCoin();
        player.addScore(200);
        EventManager::getInstance().broadcast(EventType::CoinCollected);

        spawnCoinAnimation(); // Hiện hiệu ứng đồng xu nảy lên
        
        if (coinAmount == 0) {
            // Đánh dấu khối đã dùng hết -> đổi sprite sang khối xám rỗng (Empty/Used Block)
            InteractiveBlock::hit(player); 
        }
    }
}

void CoinBlock::spawnCoinAnimation() {
    // Tạo một hiệu ứng Coin nảy lên từ vị trí block
    Vector2 spawnPos = { position.x, position.y - 32.0f };
    ItemSpawnData spawnData = { spawnPos, ItemType::Coin };
    EventManager::getInstance().broadcast(EventType::ItemSpawned, &spawnData);
}