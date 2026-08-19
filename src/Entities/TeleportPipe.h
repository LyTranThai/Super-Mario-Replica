#ifndef TELEPORT_PIPE_H
#define TELEPORT_PIPE_H

#include "Entities/StaticEntity.h"

class TeleportPipe : public StaticEntity {
public:
    TeleportPipe(Vector2 pos);
    ~TeleportPipe() override = default;

    void update(float dt) override;
    void draw() override;
    void onInteract(Player& player) override;
};

#endif // TELEPORT_PIPE_H
