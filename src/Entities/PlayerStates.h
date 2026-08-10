#ifndef PLAYER_STATES_H
#define PLAYER_STATES_H

#include "PlayerActionState.h"
#include <string>

class PlayerIdleState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerWalkState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerJumpState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerFallState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerCrouchState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerSkidState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerTakeDamageState : public PlayerActionState {
private:
    float timer = 0.0f;
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

class PlayerDieState : public PlayerActionState {
public:
    void enter(Player& player) override;
    PlayerActionState* handleInput(Player& player, const InputManager& input) override;
    PlayerActionState* update(Player& player, float dt) override;
    PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) override;
};

#endif // PLAYER_STATES_H
