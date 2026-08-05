#include "BreakableBlock.h"
#include "Player.h"
#include "PlayerPowerState.h"

void BreakableBlock::hit(Player& player) {
    std::cout << ">>> MARIO DA HUC VAO GACH! <<<\n";
    // 0. Nếu gạch đã hết item (đã bị dùng) thì không phản ứng nữa
    if (isUsed) return;

    // 1. TRƯỜNG HỢP 1: Gạch có ẩn Item bên trong (Nấm, Coin...)
    if (hiddenItem != ItemType::None) {
        bounceTimer = 0.15f; // Kích hoạt nảy theo đúng công thức sinf trong update()
        isUsed = true;       // Đánh dấu đã dùng hết item (biến thành block trống)
        
        // TODO (làm sau): Sinh Item ra màn chơi tại đây nếu cần
        // ví dụ: spawnItem(...)
        
        hiddenItem = ItemType::None;
        return;              // Có item thì KHÔNG VỠ kể cả Mario lớn
    }

    // 2. TRƯỜNG HỢP 2: Gạch thường -> Kiểm tra trạng thái Mario
    if (player.getPowerType() != PowerStateType::Small) {
        // --- MARIO LỚN HÚC -> PHÁ VỠ GẠCH ---
        setActive(false);     // Xóa khối gạch khỏi màn chơi
        player.addScore(50);  // Cộng 50 điểm cho Mario
        
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