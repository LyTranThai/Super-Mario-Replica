#ifndef FIREBALL_PLAYER_H
#define FIREBALL_PLAYER_H

#include "Player.h"
#include "SpecialMove.h"
#include <memory>

class FireballPlayer : public Player {
protected:
    float powerMoveCooldownTimer = 0.0f;
    std::unique_ptr<SpecialMove> specialMove;

public:
    FireballPlayer(Vector2 pos, const std::string& textureId, Color debugColor);
    virtual ~FireballPlayer() = default;

    void update(float dt) override;

    void setSpecialMove(std::unique_ptr<SpecialMove> move);
    SpecialMove* getSpecialMove() const { return specialMove.get(); }
    
    float getPowerMoveCooldown() const { return powerMoveCooldownTimer; }
    void resetPowerMoveCooldown(float time) { powerMoveCooldownTimer = time; }
    
    void changePowerState(PlayerPowerState *newState) override;
};

#endif // FIREBALL_PLAYER_H
