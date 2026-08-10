#ifndef BLOCK_H
#define BLOCK_H

#include "StaticEntity.h"

class Block : public StaticEntity {
public:
    enum class Type { Ground, Brick };
    Type blockType;

    Block(Vector2 pos, Type t = Type::Ground, const std::string& texID = "solid", Color dbgColor = DARKGRAY);
    ~Block() override = default;

    void update(float dt) override;
    void onInteract(Player& player) override;
    void draw() override;
};

#endif // BLOCK_H
