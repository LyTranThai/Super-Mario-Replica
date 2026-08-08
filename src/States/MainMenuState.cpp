#include "MainMenuState.h"
#include "raylib.h"
#include "Core/GameEngine.h"
#include "Core/SoundManager.h"
#include "States/GameplayState.h"
#include "States/LevelSelectState.h"
#include "States/LevelEditorState.h"
#include "States/LoginState.h"
#include "States/RegisterState.h"
#include "States/SettingsState.h"
#include "Persistence/SaveManager.h"
#include <iostream>

MainMenuState::MainMenuState() 
    : selectedIndex(0) {}

void MainMenuState::init() {
    activeAccount = GameEngine::getInstance().getActiveAccount();
    std::string charText = "CHARACTER: " + (activeAccount.getSelectedCharacter().empty() ? "MARIO" : activeAccount.getSelectedCharacter());
    
    mainOptions = { "PLAY GAME", charText, "LEVEL EDITOR", "LOGIN TO ACCOUNT", "REGISTER NEW ACCOUNT", "CUSTOM KEY SETTINGS", "QUIT" };
    
    SoundManager::getInstance().playMusic("menu_theme"); // Will play if loaded
}

void MainMenuState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::MenuUp)) {
        selectedIndex = (selectedIndex - 1 + mainOptions.size()) % mainOptions.size();
    }
    if (input.isActionJustPressed(Action::MenuDown)) {
        selectedIndex = (selectedIndex + 1) % mainOptions.size();
    }
    if (input.isActionJustPressed(Action::MenuConfirm)) {
        if (selectedIndex == 0) {
            // Play Game
            Account& currentAcc = GameEngine::getInstance().getActiveAccount();
            if (currentAcc.getUsername().empty()) {
                Account guest("Guest", "");
                guest.loadKeySettings(activeAccount.getKeySettings());
                GameEngine::getInstance().setActiveAccount(guest);
            }
            GameEngine::getInstance().getStateManager().pushState(new LevelSelectState());
        } 
        else if (selectedIndex == 1) {
            // Toggle Character Mario <-> Luigi
            Account currentAcc = GameEngine::getInstance().getActiveAccount();
            std::string curChar = currentAcc.getSelectedCharacter();
            std::string newChar = (curChar == "Luigi") ? "Mario" : "Luigi";
            currentAcc.setSelectedCharacter(newChar);
            GameEngine::getInstance().setActiveAccount(currentAcc);
            SaveManager saveMgr;
            saveMgr.saveAccount(currentAcc);

            mainOptions[1] = "CHARACTER: " + newChar;
            SoundManager::getInstance().playSound("coin");
        }
        else if (selectedIndex == 2) {
            // Level Editor
            GameEngine::getInstance().getStateManager().pushState(new LevelEditorState());
        }
        else if (selectedIndex == 3) {
            GameEngine::getInstance().getStateManager().pushState(new LoginState());
        } 
        else if (selectedIndex == 4) {
            GameEngine::getInstance().getStateManager().pushState(new RegisterState());
        } 
        else if (selectedIndex == 5) {
            GameEngine::getInstance().getStateManager().pushState(new SettingsState());
        } 
        else if (selectedIndex == 6) {
            GameEngine::getInstance().exitGame();
        }
    }
}

void MainMenuState::update(float dt) {
    (void)dt;
}

void MainMenuState::draw() {
    ClearBackground(RAYWHITE);
    
    // Draw Title
    DrawText("SUPER MARIO OOP", 150, 35, 46, MAROON);
    DrawText("C++ & Raylib Project", 250, 85, 20, DARKGRAY);

    for (size_t i = 0; i < mainOptions.size(); ++i) {
        Color color = (i == (size_t)selectedIndex) ? RED : BLACK;
        std::string prefix = (i == (size_t)selectedIndex) ? "> " : "  ";
        DrawText((prefix + mainOptions[i]).c_str(), 230, 140 + i * 45, 26, color);
    }
}

void MainMenuState::onBack() {
    // No-op: Menu has a Quit button, ESC does nothing
}
