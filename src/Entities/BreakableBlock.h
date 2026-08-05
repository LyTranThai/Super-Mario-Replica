#ifndef BREAKABLE_BLOCK_H
#define BREAKABLE_BLOCK_H

#include "InteractiveBlock.h"

class BreakableBlock : public InteractiveBlock {
private:
    bool isBroken;

public:
    BreakableBlock(Vector2 pos, ItemType item = ItemType::None);
    ~BreakableBlock() override = default;

    // Override logic khi bị Mario húc
    void hit(Player& player) override;
    
    // Hàm xử lý khi vỡ gạch (sinh ra các mảnh vỡ - Particles)
    void breakIntoPieces();
};

#endif // BREAKABLE_BLOCK_H