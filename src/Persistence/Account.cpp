#include "Account.h"
#include "raylib.h"

Account::Account() : username(""), passwordHash(""), currentLevel(1), highScore(0), selectedCharacter("Mario") {
    // Set default controls
    setKeySetting("MoveLeft", KEY_A, 0);
    setKeySetting("MoveRight", KEY_D, 0);
    setKeySetting("Jump", KEY_W, 0);
    setKeySetting("Crouch", KEY_S, 0);
    setKeySetting("Run", KEY_LEFT_SHIFT, 0);
    setKeySetting("Shoot", KEY_J, 0);
    setKeySetting("Pause", KEY_P, 0);
    setKeySetting("MenuUp", KEY_UP, 0);
    setKeySetting("MenuDown", KEY_DOWN, 0);

    // Player 2 default controls
    setKeySetting("MoveLeft", KEY_LEFT, 1);
    setKeySetting("MoveRight", KEY_RIGHT, 1);
    setKeySetting("Jump", KEY_UP, 1);
    setKeySetting("Crouch", KEY_DOWN, 1);
    setKeySetting("Run", KEY_RIGHT_SHIFT, 1);
    setKeySetting("Shoot", KEY_L, 1);
    setKeySetting("Pause", KEY_P, 1);
}

Account::Account(const std::string& user, const std::string& passHash) 
    : username(user), passwordHash(passHash), currentLevel(1), highScore(0), selectedCharacter("Mario") {
    // Set default controls
    setKeySetting("MoveLeft", KEY_A, 0);
    setKeySetting("MoveRight", KEY_D, 0);
    setKeySetting("Jump", KEY_W, 0);
    setKeySetting("Crouch", KEY_S, 0);
    setKeySetting("Run", KEY_LEFT_SHIFT, 0);
    setKeySetting("Shoot", KEY_J, 0);
    setKeySetting("Pause", KEY_P, 0);
    setKeySetting("MenuUp", KEY_UP, 0);
    setKeySetting("MenuDown", KEY_DOWN, 0);

    // Player 2 default controls
    setKeySetting("MoveLeft", KEY_LEFT, 1);
    setKeySetting("MoveRight", KEY_RIGHT, 1);
    setKeySetting("Jump", KEY_UP, 1);
    setKeySetting("Crouch", KEY_DOWN, 1);
    setKeySetting("Run", KEY_RIGHT_SHIFT, 1);
    setKeySetting("Shoot", KEY_L, 1);
    setKeySetting("Pause", KEY_P, 1);
}

void Account::setKeySetting(const std::string& actionName, int keyCode, int playerIndex) {
    std::string key = (playerIndex > 0) ? ("P" + std::to_string(playerIndex + 1) + "_" + actionName) : actionName;
    keySettings[key] = keyCode;
}

int Account::getKeySetting(const std::string& actionName, int playerIndex) const {
    std::string key = (playerIndex > 0) ? ("P" + std::to_string(playerIndex + 1) + "_" + actionName) : actionName;
    auto it = keySettings.find(key);
    if (it != keySettings.end()) {
        return it->second;
    }
    return 0;
}

void Account::loadKeySettings(const std::map<std::string, int>& settings) {
    for (auto const& [action, key] : settings) {
        keySettings[action] = key;
    }
}
