#ifndef EXIT_BLOCK_H
#define EXIT_BLOCK_H

#include "StaticEntity.h"

class ExitBlock : public StaticEntity {
public:
    ExitBlock(Vector2 pos);
    ~ExitBlock() override = default;

    void update(float dt) override;
    void draw() override;
    void onInteract(Player& player) override;
};

#endif // EXIT_BLOCK_H
