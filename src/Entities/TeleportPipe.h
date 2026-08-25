#ifndef TELEPORT_PIPE_H
#define TELEPORT_PIPE_H

#include "Entities/StaticEntity.h"

enum class PipeSize {
    Short,
    Medium,
    Long
};

class TeleportPipe : public StaticEntity {
    PipeSize pipeSize;
public:
    TeleportPipe(Vector2 pos, PipeSize size = PipeSize::Short);
    ~TeleportPipe() override = default;

    void update(float dt) override;
    void draw() override;
    void onInteract(Player& player) override;
};

#endif // TELEPORT_PIPE_H
