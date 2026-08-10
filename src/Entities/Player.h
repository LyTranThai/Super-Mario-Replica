#ifndef PLAYER_H
#define PLAYER_H

#include "DynamicEntity.h"
#include "PlayerPowerState.h"
#include "Animator.h"
#include "PlayerActionState.h"
#include "SpecialMove.h"
#include <memory>
#include <iostream>

class InputManager;

class Player : public DynamicEntity {
private:
    int lives;
    int score;
    int coins;
    int jumpCount;
    float invincibilityTimer;
    PlayerPowerState* powerState;
    std::unique_ptr<SpecialMove> specialMove;
    DynamicEntity* carriedEntity = nullptr;
    bool isCrouching;
    bool wantToStandUp;
    static constexpr float crouchHeightPercentage = 0.6f;
    std::unique_ptr<PlayerActionState> actionState;

public:
    Player(Vector2 pos);
    ~Player() override;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;

    void handleInput(const InputManager& input);
    void jump();
    int getjumpCount() const {return jumpCount;};
    void setjumpCount(int s) {jumpCount = s < 0 ? 1 : std::min(s, jumpCount);};
    void takeDamage();
    void die();
    void powerUp(PowerStateType type);
    void shootFireball();
    
    void setSpecialMove(std::unique_ptr<SpecialMove> move);
    void throwCarriedEntity();

    // Accessors
    void setCarriedEntity(DynamicEntity* entity) { carriedEntity = entity; }
    DynamicEntity* getCarriedEntity() const { return carriedEntity; }

    int getLives() const { return lives; }
    void setLives(int l) { lives = l; }

    int getScore() const { return score; }
    void addScore(int s) { score += s; }

    int getCoins() const { return coins; }
    void addCoin() { 
        coins++; 
        if (coins >= 100) {
            coins = 0;
            lives++;
        }
    }

    bool isInvincible() const { return invincibilityTimer > 0.0f; }
    PowerStateType getPowerType() const { return powerState->getType(); }
    PlayerPowerState* getPowerState() const { return powerState; }
    void changePowerState(PlayerPowerState* newState);
    void setOnGround(bool state) override;

    bool isPlayerCrouching() const { return isCrouching; }
    void setCrouching(bool state) { isCrouching = state; }
    bool getWantToStandUp() const { return wantToStandUp; }
    void setWantToStandUp(bool state) { wantToStandUp = state; }
    void getPowerStateDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const;
    void applyHitboxDimensions();
    void configureAnimations();  // Setup spritesheet frames for current power state
    Rectangle getSpriteBox() const override;
    float getJumpForce() const { return 420.0f; }
};

#endif // PLAYER_H
