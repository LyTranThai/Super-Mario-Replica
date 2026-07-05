#include "PlayerPowerState.h"
#include "Player.h"
#include "Core/InputManager.h"
#include "Core/EventSystem.h"

// --- SmallState ---
void SmallState::handleInput(Player& player, const InputManager& input) {
    Vector2 vel = player.getVelocity();
    float speedMult = player.isPlayerCrouching() ? 0.4f : 1.0f;
    if (input.isActionPressed(Action::MoveLeft)) {
        vel.x = -250.0f * speedMult;
        player.setFacingRight(false);
    } else if (input.isActionPressed(Action::MoveRight)) {
        vel.x = 250.0f * speedMult;
        player.setFacingRight(true);
    } else {
        vel.x = 0.0f; // Instant stop
    }

    if (input.isActionJustPressed(Action::Jump)) {
        player.jump();
        vel.y = player.getVelocity().y; // Sync jump velocity change to prevent copy overwrite
    }

    player.setVelocity(vel);
}

void SmallState::update(Player& player, float dt) {
    (void)player;
    (void)dt;
}

void SmallState::onDamage(Player& player) {
    // Small Mario dies immediately
    EventManager::getInstance().broadcast(EventType::PlayerDied);
}

// --- SuperState ---
void SuperState::handleInput(Player& player, const InputManager& input) {
    Vector2 vel = player.getVelocity();
    float curSpeed = input.isActionPressed(Action::Run) ? 350.0f : 250.0f; // Run speed booster
    float speedMult = player.isPlayerCrouching() ? 0.4f : 1.0f;

    if (input.isActionPressed(Action::MoveLeft)) {
        vel.x = -curSpeed * speedMult;
        player.setFacingRight(false);
    } else if (input.isActionPressed(Action::MoveRight)) {
        vel.x = curSpeed * speedMult;
        player.setFacingRight(true);
    } else {
        vel.x = 0.0f;
    }

    if (input.isActionJustPressed(Action::Jump)) {
        player.jump();
        vel.y = player.getVelocity().y; // Sync jump velocity change to prevent copy overwrite
    }

    player.setVelocity(vel);
}

void SuperState::update(Player& player, float dt) {
    (void)player;
    (void)dt;
}

void SuperState::onDamage(Player& player) {
    // Super Mario shrinks to Small state
    EventManager::getInstance().broadcast(EventType::PlayerHurt);
    player.changePowerState(new SmallState());
}

// --- FireState ---
void FireState::handleInput(Player& player, const InputManager& input) {
    Vector2 vel = player.getVelocity();
    float curSpeed = input.isActionPressed(Action::Run) ? 350.0f : 250.0f;
    float speedMult = player.isPlayerCrouching() ? 0.4f : 1.0f;

    if (input.isActionPressed(Action::MoveLeft)) {
        vel.x = -curSpeed * speedMult;
        player.setFacingRight(false);
    } else if (input.isActionPressed(Action::MoveRight)) {
        vel.x = curSpeed * speedMult;
        player.setFacingRight(true);
    } else {
        vel.x = 0.0f;
    }

    if (input.isActionJustPressed(Action::Jump)) {
        player.jump();
        vel.y = player.getVelocity().y; // Sync jump velocity change to prevent copy overwrite
    }

    player.setVelocity(vel);
}

void FireState::update(Player& player, float dt) {
    (void)player;
    (void)dt;
}

void FireState::onDamage(Player& player) {
    // Fire Mario shrinks to Super state
    EventManager::getInstance().broadcast(EventType::PlayerHurt);
    player.changePowerState(new SuperState());
}
