#include "SaveManager.h"
#include "Account.h"
#include "Core/GameEngine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <filesystem>

SaveManager::SaveManager() : saveDirectory("accounts/") {
    try {
        std::filesystem::create_directories(saveDirectory);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not create save directory: " << e.what() << std::endl;
    }
}

std::string SaveManager::getFilePath(const std::string& username) const {
    return saveDirectory + username + ".txt";
}

std::string SaveManager::simpleHash(const std::string& password) const {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(password);
    std::stringstream ss;
    ss << std::hex << hashValue;
    return ss.str();
}

bool SaveManager::registerAccount(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;

    std::string path = getFilePath(username);

    // Check if account already exists
    std::ifstream checkFile(path);
    if (checkFile.good()) {
        // Account exists
        return false;
    }
    checkFile.close();

    // Create new account
    Account newAccount(username, simpleHash(password));
    // Inherit the active controls configured on the settings menu
    newAccount.loadKeySettings(GameEngine::getInstance().getActiveAccount().getKeySettings());
    return saveAccount(newAccount);
}

bool SaveManager::verifyCredentials(const std::string& username, const std::string& password) {
    if (username.empty()) return false;

    std::string path = getFilePath(username);
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string savedUser;
    std::string savedHash;
    if (std::getline(file, savedUser) && std::getline(file, savedHash)) {
        file.close();
        return (savedUser == username && savedHash == simpleHash(password));
    }

    file.close();
    return false;
}

bool SaveManager::loadAccount(const std::string& username, Account& outAccount) {
    std::string path = getFilePath(username);
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string savedUser;
    std::string savedHash;
    if (!std::getline(file, savedUser) || !std::getline(file, savedHash)) {
        file.close();
        return false;
    }

    std::string currentLevelStr;
    std::string highScoreStr;
    if (!std::getline(file, currentLevelStr) || !std::getline(file, highScoreStr)) {
        file.close();
        return false;
    }

    int currentLevel = std::stoi(currentLevelStr);
    int highScore = std::stoi(highScoreStr);

    Account loadedAccount(savedUser, savedHash);
    loadedAccount.setCurrentLevel(currentLevel);
    loadedAccount.setHighScore(highScore);

    // Load custom key settings and level high scores
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string prefix;
        if (ss >> prefix) {
            if (prefix == "KEY") {
                std::string actionName;
                int keyCode;
                if (ss >> actionName >> keyCode) {
                    loadedAccount.setKeySetting(actionName, keyCode);
                }
            } else if (prefix == "LEVEL_SCORE") {
                int levelIdx;
                int score;
                if (ss >> levelIdx >> score) {
                    loadedAccount.setLevelHighScore(levelIdx, score);
                }
            }
        }
    }

    file.close();
    outAccount = loadedAccount;
    return true;
}

bool SaveManager::saveAccount(const Account& account) {
    std::string path = getFilePath(account.getUsername());

    try {
        // Create file
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to write account file: " << path << std::endl;
            return false;
        }

        file << account.getUsername() << "\n";
        file << account.getPasswordHash() << "\n";
        file << account.getCurrentLevel() << "\n";
        file << account.getHighScore() << "\n";

        // Write level high scores
        for (auto const& [levelIdx, score] : account.getLevelHighScores()) {
            file << "LEVEL_SCORE " << levelIdx << " " << score << "\n";
        }

        // Write bindings
        for (auto const& [action, key] : account.getKeySettings()) {
            file << "KEY " << action << " " << key << "\n";
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception while saving account: " << e.what() << std::endl;
        return false;
    }
}
