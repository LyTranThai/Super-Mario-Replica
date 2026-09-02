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
    Coin,
    Heart,
    Star,
    None
};

struct ItemSpawnData {
    Vector2 position;
    ItemType type;
};

class InteractiveBlock : public StaticEntity {
protected:
    InteractiveBlockType blockType;
    bool isUsed;
    float bounceTimer;
    Vector2 originalPosition;

public:
    InteractiveBlock(Vector2 pos, InteractiveBlockType type);
    ~InteractiveBlock() override = default;

    void update(float dt) override;
    void draw() override;
    virtual void onInteract(Player& player) override;

    virtual void hit(Player& player) = 0;
    
    bool isQuestionBlock() const { return blockType == InteractiveBlockType::Question; }
    bool isUsedUp() const { return isUsed; }
};

#endif // INTERACTIVE_BLOCK_H
