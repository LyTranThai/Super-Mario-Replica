#include "LevelSelectState.h"
#include "raylib.h"
#include "Core/GameEngine.h"
#include "Core/SoundManager.h"
#include "States/GameplayState.h"
#include <iostream>
#include <cmath>

LevelSelectState::LevelSelectState() : selectedIndex(0), animTime(0.0f)
{
    levelFiles = {"assets/levels/level1.txt", "assets/levels/level2.txt", "assets/levels/level3.txt", "assets/levels/custom_level.txt", "RANDOM"};
}

void LevelSelectState::init()
{
    // Only play menu theme if it's not already playing
    if (!SoundManager::getInstance().isMusicPlaying("menu_theme"))
    {
        SoundManager::getInstance().playMusic("menu_theme");
    }
}

void LevelSelectState::handleInput(const InputManager &input)
{
    if (input.isActionJustPressed(Action::MoveRight) || IsKeyPressed(KEY_RIGHT))
    {
        selectedIndex = (selectedIndex + 1) % levelFiles.size();
    }
    if (input.isActionJustPressed(Action::MoveLeft) || IsKeyPressed(KEY_LEFT))
    {
        selectedIndex = (selectedIndex - 1 + levelFiles.size()) % levelFiles.size();
    }
    if (input.isActionJustPressed(Action::MenuConfirm))
    {
        Account &acc = GameEngine::getInstance().getActiveAccount();
        bool isLocked = (selectedIndex < 3 && selectedIndex >= acc.getCurrentLevel());
        if (isLocked)
        {
            SoundManager::getInstance().playSound("break");
        }
        else
        {
            GameEngine::getInstance().getStateManager().pushState(new GameplayState(selectedIndex));
        }
    }
}

void LevelSelectState::update(float dt)
{
    animTime += dt;
}

void LevelSelectState::onBack()
{
    GameEngine::getInstance().getStateManager().popState();
}

void LevelSelectState::draw()
{
    // Sleek dark arcade background
    ClearBackground(Color{15, 15, 20, 255});

    // Draw subtle grid overlay
    for (int i = 0; i < GetScreenWidth(); i += 40)
    {
        DrawLine(i, 0, i, GetScreenHeight(), Color{40, 40, 50, 25});
    }
    for (int i = 0; i < GetScreenHeight(); i += 40)
    {
        DrawLine(0, i, GetScreenWidth(), i, Color{40, 40, 50, 25});
    }

    // Title
    DrawText("SUPER MARIO OOP", 220, 25, 36, MAROON);
    DrawText("LEVEL SELECTION", 310, 70, 20, GOLD);

    Account &acc = GameEngine::getInstance().getActiveAccount();

    for (size_t i = 0; i < levelFiles.size(); ++i)
    {
        int cardX = 20 + i * 152;
        int cardY = 125;
        int cardW = 142;
        int cardH = 360;

        bool isSelected = (i == (size_t)selectedIndex);
        bool isCustom = (i == 3);
        bool isRandom = (i == 4);
        bool isLocked = (!isCustom && !isRandom && i >= (size_t)acc.getCurrentLevel());
        int highScore = acc.getLevelHighScore(i);

        Color bgColor;
        Color borderNormalColor;

        if (isLocked)
        {
            bgColor = Color{30, 20, 20, 255};
            borderNormalColor = Color{80, 40, 40, 255};
        }
        else if (isCustom)
        {
            bgColor = Color{35, 25, 45, 255};
            borderNormalColor = Color{120, 50, 160, 255};
        }
        else if (isRandom)
        {
            bgColor = Color{20, 40, 35, 255};
            borderNormalColor = Color{40, 180, 120, 255};
        }
        else if (highScore > 0)
        {
            bgColor = Color{25, 35, 45, 255};
            borderNormalColor = Color{50, 100, 150, 255};
        }
        else
        {
            bgColor = Color{30, 30, 35, 255};
            borderNormalColor = Color{80, 80, 90, 255};
        }

        float scaleOffset = 0.0f;
        Color borderColor = borderNormalColor;
        if (isSelected)
        {
            scaleOffset = sin(animTime * 6.0f) * 4.0f;
            borderColor = isLocked ? RED : GOLD;
        }

        // Draw card bounds
        Rectangle cardRect = {
            (float)cardX - scaleOffset / 2.0f,
            (float)cardY - scaleOffset / 2.0f,
            (float)cardW + scaleOffset,
            (float)cardH + scaleOffset};

        DrawRectangleRounded(cardRect, 0.08f, 4, bgColor);
        DrawRectangleRoundedLines(cardRect, 0.08f, 4, isSelected ? 4.0f : 2.0f, borderColor);

        // Render content
        std::string levelTitle = isRandom ? "RANDOM" : (isCustom ? "CUSTOM" : ("LEVEL " + std::to_string(i + 1)));
        DrawText(levelTitle.c_str(), cardX + 15, cardY + 25, 20, isLocked ? GRAY : WHITE);

        if (isLocked)
        {
            DrawText("LOCKED", cardX + 30, cardY + 110, 18, RED);
            DrawText("Requirement:", cardX + 10, cardY + 170, 12, LIGHTGRAY);
            std::string reqMsg = "Beat Level " + std::to_string(i);
            DrawText(reqMsg.c_str(), cardX + 10, cardY + 195, 12, GRAY);
            DrawText("(Locked)", cardX + 40, cardY + 280, 14, DARKGRAY);
        }
        else if (isCustom)
        {
            DrawText("USER MAP", cardX + 25, cardY + 110, 16, PURPLE);
            DrawText("Custom Level", cardX + 10, cardY + 170, 12, LIGHTGRAY);
            DrawText("Created in Editor", cardX + 5, cardY + 195, 12, SKYBLUE);
            DrawText("(Play)", cardX + 48, cardY + 280, 14, GOLD);
        }
        else if (isRandom)
        {
            int tWidth1 = MeasureText("MULTIPLAYER-", 13);
            int tWidth2 = MeasureText("INFINITE", 13);
            DrawText("MULTIPLAYER-", cardX + (cardW - tWidth1) / 2, cardY + 102, 13, Color{40, 200, 120, 255});
            DrawText("INFINITE", cardX + (cardW - tWidth2) / 2, cardY + 120, 13, Color{40, 200, 120, 255});
            DrawText("Procedural Map", cardX + 8, cardY + 170, 12, LIGHTGRAY);
            DrawText("Random Seed!", cardX + 10, cardY + 195, 12, GREEN);
            DrawText("(Play)", cardX + 48, cardY + 280, 14, GOLD);
        }
        else
        {
            if (highScore > 0)
            {
                DrawText("COMPLETED", cardX + 15, cardY + 100, 16, GREEN);
                DrawText("Highest Score:", cardX + 10, cardY + 160, 12, LIGHTGRAY);
                std::string scoreStr = std::to_string(highScore);
                DrawText(scoreStr.c_str(), cardX + 10, cardY + 190, 20, GOLD);
                DrawText("(Replay)", cardX + 40, cardY + 280, 14, GRAY);
            }
            else
            {
                DrawText("UNLOCKED", cardX + 22, cardY + 100, 16, SKYBLUE);
                DrawText("Haven't played", cardX + 10, cardY + 160, 12, LIGHTGRAY);
                DrawText("(New)", cardX + 48, cardY + 280, 14, GRAY);
            }
        }
    }

    // Bottom prompt
    DrawText("Press [LEFT/RIGHT] to Navigate  |  [ENTER] to Play  |  [ESC] to return", 80, 520, 18, LIGHTGRAY);
}
