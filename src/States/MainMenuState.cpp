#include "MainMenuState.hpp"
#include "raylib.h"
#include "Core/GameEngine.hpp"
#include "Core/SoundManager.hpp"
#include "States/GameplayState.hpp"
#include "States/LevelSelectState.hpp"
#include "States/LoginState.hpp"
#include "States/RegisterState.hpp"
#include "States/SettingsState.hpp"
#include <iostream>

MainMenuState::MainMenuState() 
    : selectedIndex(0) {}

void MainMenuState::init() {
    mainOptions = { "PLAY AS GUEST", "LOGIN TO ACCOUNT", "REGISTER NEW ACCOUNT", "CUSTOM KEY SETTINGS", "QUIT" };
    
    // Default account to engine settings
    activeAccount = GameEngine::getInstance().getActiveAccount();
    
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
            // Guest play
            Account guest("Guest", "");
            guest.loadKeySettings(activeAccount.getKeySettings()); // Inherit custom menu controls
            GameEngine::getInstance().setActiveAccount(guest);
            
            // Rebind keys in engine key bindings to match default guest configurations
            for (auto const& [actName, keyCode] : guest.getKeySettings()) {
                Action act;
                if (actName == "MoveLeft") act = Action::MoveLeft;
                else if (actName == "MoveRight") act = Action::MoveRight;
                else if (actName == "Jump") act = Action::Jump;
                else if (actName == "Crouch") act = Action::Crouch;
                else if (actName == "Run") act = Action::Run;
                else if (actName == "Shoot") act = Action::Shoot;
                else if (actName == "Pause") act = Action::Pause;
                else if (actName == "MenuUp") act = Action::MenuUp;
                else if (actName == "MenuDown") act = Action::MenuDown;
                else continue;
                GameEngine::getInstance().getInputManager().bindKey(keyCode, act);
            }
            
            GameEngine::getInstance().getStateManager().pushState(new LevelSelectState());
        } 
        else if (selectedIndex == 1) {
            GameEngine::getInstance().getStateManager().pushState(new LoginState());
        } 
        else if (selectedIndex == 2) {
            GameEngine::getInstance().getStateManager().pushState(new RegisterState());
        } 
        else if (selectedIndex == 3) {
            GameEngine::getInstance().getStateManager().pushState(new SettingsState());
        } 
        else if (selectedIndex == 4) {
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
    DrawText("SUPER MARIO OOP", 150, 50, 50, MAROON);
    DrawText("C++ & Raylib Project", 250, 110, 20, DARKGRAY);

    for (size_t i = 0; i < mainOptions.size(); ++i) {
        Color color = (i == (size_t)selectedIndex) ? RED : BLACK;
        std::string prefix = (i == (size_t)selectedIndex) ? "> " : "  ";
        DrawText((prefix + mainOptions[i]).c_str(), 250, 200 + i * 50, 30, color);
    }
}

void MainMenuState::onBack() {
    // No-op: Menu has a Quit button, ESC does nothing
}
