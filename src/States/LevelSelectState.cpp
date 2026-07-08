#include "LevelSelectState.hpp"
#include "raylib.h"
#include "Core/GameEngine.hpp"
#include "Core/SoundManager.hpp"
#include "States/GameplayState.hpp"
#include <iostream>
#include <cmath>

LevelSelectState::LevelSelectState() : selectedIndex(0), animTime(0.0f) {
    levelFiles = { "assets/levels/level1.txt", "assets/levels/level2.txt", "assets/levels/level3.txt" };
}

void LevelSelectState::init() {
    SoundManager::getInstance().playMusic("menu_theme");
}

void LevelSelectState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::MoveRight) || IsKeyPressed(KEY_RIGHT)) {
        selectedIndex = (selectedIndex + 1) % levelFiles.size();
    }
    if (input.isActionJustPressed(Action::MoveLeft) || IsKeyPressed(KEY_LEFT)) {
        selectedIndex = (selectedIndex - 1 + levelFiles.size()) % levelFiles.size();
    }
    if (input.isActionJustPressed(Action::MenuConfirm)) {
        Account& acc = GameEngine::getInstance().getActiveAccount();
        bool isLocked = (selectedIndex >= acc.getCurrentLevel());
        if (isLocked) {
            SoundManager::getInstance().playSound("break");
        } else {
            GameEngine::getInstance().getStateManager().pushState(new GameplayState(selectedIndex));
        }
    }
}

void LevelSelectState::update(float dt) {
    animTime += dt;
}

void LevelSelectState::onBack() {
    GameEngine::getInstance().getStateManager().popState();
}

void LevelSelectState::draw() {
    // Sleek dark arcade background
    ClearBackground(Color{ 15, 15, 20, 255 });
    
    // Draw subtle grid overlay
    for (int i = 0; i < GetScreenWidth(); i += 40) {
        DrawLine(i, 0, i, GetScreenHeight(), Color{ 40, 40, 50, 25 });
    }
    for (int i = 0; i < GetScreenHeight(); i += 40) {
        DrawLine(0, i, GetScreenWidth(), i, Color{ 40, 40, 50, 25 });
    }

    // Title
    DrawText("SUPER MARIO OOP", 220, 40, 40, MAROON);
    DrawText("LEVEL SELECTION", 310, 95, 22, GOLD);

    Account& acc = GameEngine::getInstance().getActiveAccount();

    for (size_t i = 0; i < levelFiles.size(); ++i) {
        int cardX = 40 + i * 250;
        int cardY = 160;
        int cardW = 220;
        int cardH = 320;
        
        bool isSelected = (i == (size_t)selectedIndex);
        bool isLocked = (i >= (size_t)acc.getCurrentLevel());
        int highScore = acc.getLevelHighScore(i);
        
        Color bgColor;
        Color borderNormalColor;
        
        if (isLocked) {
            bgColor = Color{ 30, 20, 20, 255 };
            borderNormalColor = Color{ 80, 40, 40, 255 };
        } else if (highScore > 0) {
            bgColor = Color{ 25, 35, 45, 255 };
            borderNormalColor = Color{ 50, 100, 150, 255 };
        } else {
            bgColor = Color{ 30, 30, 35, 255 };
            borderNormalColor = Color{ 80, 80, 90, 255 };
        }
        
        float scaleOffset = 0.0f;
        Color borderColor = borderNormalColor;
        if (isSelected) {
            scaleOffset = sin(animTime * 6.0f) * 4.0f;
            borderColor = isLocked ? RED : GOLD;
        }
        
        // Draw card bounds
        Rectangle cardRect = { 
            (float)cardX - scaleOffset / 2.0f, 
            (float)cardY - scaleOffset / 2.0f, 
            (float)cardW + scaleOffset, 
            (float)cardH + scaleOffset 
        };
        
        DrawRectangleRounded(cardRect, 0.08f, 4, bgColor);
        DrawRectangleRoundedLines(cardRect, 0.08f, 4, isSelected ? 4.0f : 2.0f, borderColor);
        
        // Render content
        std::string levelTitle = "LEVEL " + std::to_string(i + 1);
        DrawText(levelTitle.c_str(), cardX + 55, cardY + 30, 28, isLocked ? GRAY : WHITE);
        
        if (isLocked) {
            DrawText("LOCKED", cardX + 60, cardY + 120, 24, RED);
            DrawText("Requirement:", cardX + 30, cardY + 180, 16, LIGHTGRAY);
            std::string reqMsg = "Beat Level " + std::to_string(i);
            DrawText(reqMsg.c_str(), cardX + 30, cardY + 205, 16, GRAY);
            DrawText("(Locked)", cardX + 75, cardY + 260, 16, DARKGRAY);
        } else {
            if (highScore > 0) {
                DrawText("COMPLETED", cardX + 45, cardY + 100, 20, GREEN);
                DrawText("Highest Score:", cardX + 30, cardY + 160, 16, LIGHTGRAY);
                std::string scoreStr = std::to_string(highScore);
                DrawText(scoreStr.c_str(), cardX + 30, cardY + 190, 24, GOLD);
                DrawText("(Replay)", cardX + 75, cardY + 260, 16, GRAY);
            } else {
                DrawText("UNLOCKED", cardX + 50, cardY + 100, 20, SKYBLUE);
                DrawText("Haven't played", cardX + 30, cardY + 160, 16, LIGHTGRAY);
                DrawText("(New)", cardX + 85, cardY + 260, 16, GRAY);
            }
        }
    }

    // Bottom prompt
    DrawText("Press [LEFT/RIGHT] to Navigate  |  [ENTER] to Play  |  [ESC] to return", 80, 520, 18, LIGHTGRAY);
}
