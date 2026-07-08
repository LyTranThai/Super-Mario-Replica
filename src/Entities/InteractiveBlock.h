#ifndef INTERACTIVE_BLOCK_H
#define INTERACTIVE_BLOCK_H

#include "StaticEntity.h"

enum class InteractiveBlockType {
    Brick,
    Question
};

enum class ItemType {
    Mushroom,
    FireFlower,
    Star,
    Coin,
    None
};

class InteractiveBlock : public StaticEntity {
private:
    InteractiveBlockType blockType;
    ItemType hiddenItem;
    bool isUsed;
    float bounceTimer;
    Vector2 originalPosition;

public:
    InteractiveBlock(Vector2 pos, InteractiveBlockType type, ItemType item = ItemType::None);
    ~InteractiveBlock() override = default;

    void update(float dt) override;
    void draw() override;
    void onInteract(Player& player) override; // Hitting from below

    void hit(Player& player);
    
    bool isQuestionBlock() const { return blockType == InteractiveBlockType::Question; }
    bool isUsedUp() const { return isUsed; }
};

#endif // INTERACTIVE_BLOCK_H
