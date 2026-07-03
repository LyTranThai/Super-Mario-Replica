#ifndef PLAYER_H
#define PLAYER_H

#include "DynamicEntity.h"
#include "PlayerPowerState.h"

class InputManager;

class Player : public DynamicEntity {
private:
    int lives;
    int score;
    int coins;
    int jumpCount;
    float invincibilityTimer;
    PlayerPowerState* powerState;
    bool isCrouching;
    bool wantToStandUp;
    static constexpr float crouchHeightPercentage = 0.6f;

    // Movement rates constants
    static constexpr float speed = 250.0f;
    static constexpr float jumpForce = 420.0f;

public:
    Player(Vector2 pos);
    ~Player() override;

    void update(float dt) override;
    void draw() override;
    void onCollision(Entity& other, CollisionSide side) override;

    void handleInput(const InputManager& input);
    void jump();
    void doubleJump();
    void takeDamage();
    void powerUp(PowerStateType type);
    void shootFireball();

    // Accessors
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
    void changePowerState(PlayerPowerState* newState);
    void setOnGround(bool state) override;

    bool isPlayerCrouching() const { return isCrouching; }
    void setCrouching(bool state) { isCrouching = state; }
    bool getWantToStandUp() const { return wantToStandUp; }
    void setWantToStandUp(bool state) { wantToStandUp = state; }
    void getPowerStateDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const;
    void applyHitboxDimensions();
    Rectangle getSpriteBox() const override;
};

#endif // PLAYER_H
