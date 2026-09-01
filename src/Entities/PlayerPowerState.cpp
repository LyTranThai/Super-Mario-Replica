#include "PlayerPowerState.h"
#include "Player.h"
#include "Core/InputManager.h"
#include "Core/EventSystem.h"

// --- SmallState ---
void SmallState::handleInput(Player& player, const InputManager& input) {
    (void)player;
    (void)input;
}

void SmallState::update(Player& player, float dt) {
    (void)player;
    (void)dt;
}

void SmallState::onDamage(Player& player) {
    // If Small Mario gets hit, we decrease a life but don't reset the level unless it's game over.
    if (player.getLives() > 1) {
        player.setLives(player.getLives() - 1);
        // Player continues playing, invincibilityTimer handles the flickering.
    } else {
        // Game Over condition
        EventManager::getInstance().broadcast(EventType::PlayerDied);
    }
}

// --- SuperState ---
void SuperState::handleInput(Player& player, const InputManager& input) {
    (void)player;
    (void)input;
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

#include "FireballPlayer.h"

// --- FireState ---
void FireState::handleInput(Player& player, const InputManager& input) {
    if (input.isActionJustPressed(Action::Shoot)) {
        if (player.getCarriedEntity() != nullptr) {
            return;
        }
        if (auto* fbPlayer = dynamic_cast<FireballPlayer*>(&player)) {
            if (fbPlayer->getPowerMoveCooldown() <= 0.0f) {
                if (fbPlayer->getSpecialMove()) {
                    fbPlayer->getSpecialMove()->execute(*fbPlayer);
                }
            }
        }
    }
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
