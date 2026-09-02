#include "BreakableBlock.h"
#include "Player.h"
#include "PlayerPowerState.h"
#include "Core/EventSystem.h"
#include <iostream>

BreakableBlock::BreakableBlock(Vector2 pos)
    : InteractiveBlock(pos, InteractiveBlockType::Brick), isBroken(false) {}

void BreakableBlock::hit(Player& player) {
    std::cout << ">>> MARIO DA HUC VAO GACH! <<<\n";
    if (isUsed) return;

    if (player.getPowerType() != PowerStateType::Small) {
        // --- MARIO LỚN HÚC -> PHÁ VỠ GẠCH ---
        setActive(false);
        EventManager::getInstance().broadcast(EventType::BrickBroken);
        player.addScore(50);
        
        // TODO (làm sau): Tạo 4 mảnh vỡ bay ra (Debris) + Âm thanh vỡ gạch
    } else {
        // --- MARIO NHỎ HÚC -> CHỈ NẢY LÊN ---
        bounceTimer = 0.15f;  // Kích hoạt hiệu ứng nảy lên rớt xuống
        
        // TODO (làm sau): Âm thanh "bump" khi húc gạch
    }
}

void BreakableBlock::breakIntoPieces() {
    isBroken = true;
    // TODO: Spawn 4 mảnh vỡ gạch văng ra 4 hướng & hủy đối tượng này
}