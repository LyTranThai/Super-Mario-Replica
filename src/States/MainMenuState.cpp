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
#include "Core/AssetManager.h"
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
                guest.setSelectedCharacter(currentAcc.getSelectedCharacter());
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
    
    Texture2D menuTex = AssetManager::getInstance().getTexture("mainmenu");
    float menuStartY = 140.0f;
    float optionSpacing = 45.0f;

    if (menuTex.id != 0) {
        // Draw banner logo from mainmenu.png based on assets/textures/readme.md:
        // Position: (233, 28) -> (424, 115)
        Rectangle source = { 233.0f, 28.0f, 192.0f, 88.0f };
        float scale = 2.0f;
        float destWidth = source.width * scale;
        float destHeight = source.height * scale;
        float destX = (GetScreenWidth() - destWidth) / 2.0f;
        float destY = 20.0f;
        Rectangle dest = { destX, destY, destWidth, destHeight };
        DrawTexturePro(menuTex, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        menuStartY = destY + destHeight + 20.0f;
        optionSpacing = 42.0f;
    } else {
        // Fallback title
        DrawText("SUPER MARIO OOP", 150, 35, 46, MAROON);
        DrawText("C++ & Raylib Project", 250, 85, 20, DARKGRAY);
    }

    for (size_t i = 0; i < mainOptions.size(); ++i) {
        Color color = (i == (size_t)selectedIndex) ? RED : BLACK;
        std::string prefix = (i == (size_t)selectedIndex) ? "> " : "  ";
        DrawText((prefix + mainOptions[i]).c_str(), 230, (int)(menuStartY + i * optionSpacing), 26, color);
    }

    const char* hint = "Use UP/DOWN to navigate  |  ENTER to select";
    int hintWidth = MeasureText(hint, 18);
    DrawText(hint, (GetScreenWidth() - hintWidth) / 2, 545, 18, DARKGRAY);
}

void MainMenuState::onBack() {
    // No-op: Menu has a Quit button, ESC does nothing
}
