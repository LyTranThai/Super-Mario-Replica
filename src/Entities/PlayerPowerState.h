#ifndef PLAYER_POWER_STATE_H
#define PLAYER_POWER_STATE_H

#include "raylib.h"

class Player;

enum class PowerStateType {
    Small,
    Super,
    Fire
};

class PlayerPowerState {
public:
    virtual ~PlayerPowerState() = default;
    virtual PowerStateType getType() const = 0;
    virtual void onDamage(Player& player) = 0;
    virtual void getDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const = 0;
    virtual float getMaxSpeed(bool isRunning) const = 0;
};

// --- Concrete Powerup States ---

class SmallState : public PlayerPowerState {
public:
    PowerStateType getType() const override { return PowerStateType::Small; }
    void onDamage(Player& player) override;
    void getDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const override {
        outSpriteSize = Vector2{ 32.0f, 32.0f };
        outHitboxSize = Vector2{ 20.0f, 26.0f };
        outHitboxOffset = Vector2{ 6.0f, 6.0f }; // Centered
    }
    float getMaxSpeed(bool isRunning) const override { return isRunning ? 350.0f : 250.0f; }
};

class SuperState : public PlayerPowerState {
public:
    PowerStateType getType() const override { return PowerStateType::Super; }
    void onDamage(Player& player) override;
    void getDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const override {
        outSpriteSize = Vector2{ 32.0f, 64.0f };
        outHitboxSize = Vector2{ 20.0f, 56.0f };
        outHitboxOffset = Vector2{ 6.0f, 8.0f };
    }
    float getMaxSpeed(bool isRunning) const override { return isRunning ? 350.0f : 250.0f; }
};

class FireState : public PlayerPowerState {
public:
    PowerStateType getType() const override { return PowerStateType::Fire; }
    void onDamage(Player& player) override;
    void getDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const override {
        outSpriteSize = Vector2{ 32.0f, 64.0f };
        outHitboxSize = Vector2{ 20.0f, 56.0f };
        outHitboxOffset = Vector2{ 6.0f, 8.0f };
    }
    float getMaxSpeed(bool isRunning) const override { return isRunning ? 350.0f : 250.0f; }
};

#endif // PLAYER_POWER_STATE_H
