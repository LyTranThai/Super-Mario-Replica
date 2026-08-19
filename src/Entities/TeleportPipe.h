#ifndef TELEPORT_PIPE_H
#define TELEPORT_PIPE_H

#include "Entities/StaticEntity.h"

class TeleportPipe : public StaticEntity {
    bool isLong;
public:
    TeleportPipe(Vector2 pos, bool isLong = false);
    ~TeleportPipe() override = default;

    void update(float dt) override;
    void draw() override;
    void onInteract(Player& player) override;
};

#endif // TELEPORT_PIPE_H
