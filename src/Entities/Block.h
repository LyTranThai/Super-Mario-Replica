#ifndef BLOCK_H
#define BLOCK_H

#include "StaticEntity.h"

class Block : public StaticEntity {
public:
    enum class Type { Ground, Brick };
    Type blockType;
    bool isTopGround;

    Block(Vector2 pos, Type t = Type::Ground, bool top = true, const std::string& texID = "world", Color dbgColor = DARKGRAY);
    ~Block() override = default;

    void update(float dt) override;
    void onInteract(Player& player) override;
    void draw() override;
};

#endif // BLOCK_H
