#include "PlayerPowerState.h"
#include "Player.h"
#include "Core/EventSystem.h"

// --- SmallState ---
void SmallState::onDamage(Player& player) {
    // Small Mario dies immediately
    EventManager::getInstance().broadcast(EventType::PlayerDied);
}

// --- SuperState ---
void SuperState::onDamage(Player& player) {
    // Super Mario shrinks to Small state
    EventManager::getInstance().broadcast(EventType::PlayerHurt);
    player.changePowerState(new SmallState());
}

// --- FireState ---
void FireState::onDamage(Player& player) {
    // Fire Mario shrinks to Super state
    EventManager::getInstance().broadcast(EventType::PlayerHurt);
    player.changePowerState(new SuperState());
}
