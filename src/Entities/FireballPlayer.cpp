#include "FireballPlayer.h"
#include "SpecialMove.h"

FireballPlayer::FireballPlayer(Vector2 pos, const std::string& textureId, Color debugColor)
    : Player(pos, textureId, debugColor) {
    specialMove = std::make_unique<NoneMove>();
}

void FireballPlayer::update(float dt) {
    Player::update(dt);
    
    if (!isPiping()) {
        if (powerMoveCooldownTimer > 0.0f) {
            powerMoveCooldownTimer -= dt;
            if (powerMoveCooldownTimer < 0.0f) {
                powerMoveCooldownTimer = 0.0f;
            }
        }
    }
}

void FireballPlayer::setSpecialMove(std::unique_ptr<SpecialMove> move) {
    specialMove = std::move(move);
}

void FireballPlayer::changePowerState(PlayerPowerState *newState) {
    Player::changePowerState(newState);
    
    if (getPowerType() == PowerStateType::Fire || getPowerType() == PowerStateType::Super) {
        setSpecialMove(std::make_unique<FireballMove>());
    } else {
        setSpecialMove(std::make_unique<NoneMove>());
    }
}
