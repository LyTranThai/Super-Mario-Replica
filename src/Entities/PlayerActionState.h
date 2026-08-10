#ifndef PLAYER_ACTION_STATE_H
#define PLAYER_ACTION_STATE_H

#include "raylib.h"
#include "Entity.h"

class Player;
class InputManager;

class PlayerActionState {
public:
    virtual ~PlayerActionState() = default;

    // Called once when entering the state
    virtual void enter(Player& player) = 0;

    // Returns a new state if input causes a transition, nullptr otherwise
    virtual PlayerActionState* handleInput(Player& player, const InputManager& input) = 0;

    // Returns a new state if physics/flags cause a transition, nullptr otherwise
    virtual PlayerActionState* update(Player& player, float dt) = 0;

    // Returns a new state if a specific collision causes a transition, nullptr otherwise
    virtual PlayerActionState* onCollision(Player& player, Entity& other, CollisionSide side) {
        return nullptr;
    }
};

#endif // PLAYER_ACTION_STATE_H
