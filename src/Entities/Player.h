#ifndef PLAYER_H
#define PLAYER_H

#include "DynamicEntity.h"
#include "PlayerPowerState.h"
#include "SpriteAnimator.h"
#include "Core/EventSystem.h"
#include <memory>
#include <iostream>

class InputManager;

class Player : public DynamicEntity
{
protected:
    int lives;
    int score;
    int coins;
    int jumpCount;
    float invincibilityTimer;
    PlayerPowerState *powerState;
    DynamicEntity *carriedEntity = nullptr;
    bool isCrouching;
    bool wantToStandUp;
    bool isPipingFlag = false;
    float pipingTimer = 0.0f;
    Vector2 pipeTargetPos;
    bool isExitPipe = false;
    static constexpr float crouchHeightPercentage = 1.0f;
    SpriteAnimator animator;

    // Movement rates constants
    float speed = 250.0f;
    float jumpForce = 420.0f;

public:
    Player(Vector2 pos, const std::string& textureId, Color debugColor);
    virtual ~Player() override;

    virtual void update(float dt) override;
    virtual void draw() override;
    virtual void onCollision(Entity &other, CollisionSide side) override;

    virtual void handleInput(const InputManager &input);
    virtual void jump();
    int getjumpCount() const { return jumpCount; };
    void setjumpCount(int s) { jumpCount = s < 0 ? 1 : std::min(s, jumpCount); };
    virtual void takeDamage();
    virtual void powerUp(PowerStateType type);
    void throwCarriedEntity();

    // Accessors
    void setCarriedEntity(DynamicEntity *entity) { carriedEntity = entity; }
    DynamicEntity *getCarriedEntity() const { return carriedEntity; }

    int getLives() const { return lives; }
    void setLives(int l) { lives = l; }

    int getScore() const { return score; }
    void addScore(int s) { score += s; }

    int getCoins() const { return coins; }
    void addCoin()
    {
        coins++;
        if (coins >= 100)
        {
            coins = 0;
            lives++;
        }
    }

    bool isInvincible() const { return invincibilityTimer > 0.0f; }
    Vector2 getHitboxOffset() const { return hitboxOffset; }
    PowerStateType getPowerType() const { return powerState->getType(); }
    virtual void changePowerState(PlayerPowerState *newState);
    void setOnGround(bool state) override;

    bool isPlayerCrouching() const { return isCrouching; }
    void setCrouching(bool state) { isCrouching = state; }
    bool getWantToStandUp() const { return wantToStandUp; }
    void setWantToStandUp(bool state) { wantToStandUp = state; }

    bool isPiping() const { return isPipingFlag; }
    void startPiping(Vector2 target, bool isExit)
    {
        if (isPipingFlag)
            return;
        isPipingFlag = true;
        pipingTimer = 0.0f;
        pipeTargetPos = target;
        isExitPipe = isExit;
        velocity = {0.0f, 0.0f};
        isCrouching = false;
        wantToStandUp = false;
        EventManager::getInstance().broadcast(EventType::PipeWarp);
    }

    void getPowerStateDimensions(Vector2 &outSpriteSize, Vector2 &outHitboxSize, Vector2 &outHitboxOffset) const;
    void applyHitboxDimensions();
    virtual void configureAnimations() = 0;  // Setup spritesheet frames for current power state
    void updateAnimationState(); // Pick animation based on physics state
    Rectangle getSpriteBox() const override;
    
    virtual Texture2D getSpriteTexture() const;
    virtual Color getSpriteTint() const { return WHITE; }
};

#endif // PLAYER_H
