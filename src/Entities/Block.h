#ifndef BLOCK_H
#define BLOCK_H

#include "StaticEntity.h"

class Block : public StaticEntity {
public:
    Block(Vector2 pos, const std::string& texID, Color dbgColor = DARKGRAY);
    ~Block() override = default;

    void update(float dt) override;
    void onInteract(Player& player) override;
};

#endif // BLOCK_H
