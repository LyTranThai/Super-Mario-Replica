#include "Account.h"
#include "raylib.h"

Account::Account() : username(""), passwordHash(""), currentLevel(1), highScore(0) {
    // Set default controls
    setKeySetting("MoveLeft", KEY_A);
    setKeySetting("MoveRight", KEY_D);
    setKeySetting("Jump", KEY_W);
    setKeySetting("Crouch", KEY_S);
    setKeySetting("Run", KEY_LEFT_SHIFT);
    setKeySetting("Shoot", KEY_J);
    setKeySetting("Pause", KEY_P);
    setKeySetting("MenuUp", KEY_UP);
    setKeySetting("MenuDown", KEY_DOWN);
}

Account::Account(const std::string& user, const std::string& passHash) 
    : username(user), passwordHash(passHash), currentLevel(1), highScore(0) {
    // Set default controls
    setKeySetting("MoveLeft", KEY_A);
    setKeySetting("MoveRight", KEY_D);
    setKeySetting("Jump", KEY_W);
    setKeySetting("Crouch", KEY_S);
    setKeySetting("Run", KEY_LEFT_SHIFT);
    setKeySetting("Shoot", KEY_J);
    setKeySetting("Pause", KEY_P);
    setKeySetting("MenuUp", KEY_UP);
    setKeySetting("MenuDown", KEY_DOWN);
}

void Account::setKeySetting(const std::string& actionName, int keyCode) {
    keySettings[actionName] = keyCode;
}

int Account::getKeySetting(const std::string& actionName) const {
    auto it = keySettings.find(actionName);
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
