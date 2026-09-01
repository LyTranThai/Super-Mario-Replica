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
#include "States/AudioSettingsState.h"
#include "Persistence/SaveManager.h"
#include "Core/AssetManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MainMenuState::MainMenuState() 
    : selectedIndex(0), animTime(0.0f) {}

void MainMenuState::updateMenuOptionTexts() {
    activeAccount = GameEngine::getInstance().getActiveAccount();
    std::string charText = "CHARACTER: " + (activeAccount.getSelectedCharacter().empty() ? "MARIO" : activeAccount.getSelectedCharacter());
    if (mainOptions.size() > 1) {
        mainOptions[1] = charText;
    }

    SoundManager& sm = SoundManager::getInstance();
    std::string audioText;
    if (sm.isMuted()) {
        audioText = "AUDIO SETTINGS: [MUTED]";
    } else {
        int volPercent = (int)std::round(sm.getMasterVolume() * 100.0f);
        audioText = "AUDIO SETTINGS: < " + std::to_string(volPercent) + "% >";
    }
    if (mainOptions.size() > 3) {
        mainOptions[3] = audioText;
    }
}

void MainMenuState::init() {
    activeAccount = GameEngine::getInstance().getActiveAccount();
    
    mainOptions = {
        "PLAY GAME",
        "CHARACTER: MARIO",
        "LEVEL EDITOR",
        "AUDIO SETTINGS",
        "CUSTOM KEY SETTINGS",
        "LOGIN TO ACCOUNT",
        "REGISTER NEW ACCOUNT",
        "QUIT"
    };

    updateMenuOptionTexts();
    SoundManager::getInstance().playMusic("menu_theme"); // Will play if loaded
}

void MainMenuState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::MenuUp) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + (int)mainOptions.size()) % (int)mainOptions.size();
    }
    if (input.isActionJustPressed(Action::MenuDown) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % (int)mainOptions.size();
    }

    // Quick volume adjustment right from the main menu when highlighting Audio Settings
    if (selectedIndex == 3) {
        if (input.isActionJustPressed(Action::MoveLeft) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            SoundManager& sm = SoundManager::getInstance();
            if (sm.isMuted()) sm.setMuted(false);
            float newVol = std::max(0.0f, sm.getMasterVolume() - 0.10f);
            sm.setMasterVolume(newVol);
            updateMenuOptionTexts();
            sm.playSound("coin");
        }
        else if (input.isActionJustPressed(Action::MoveRight) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            SoundManager& sm = SoundManager::getInstance();
            if (sm.isMuted()) sm.setMuted(false);
            float newVol = std::min(1.0f, sm.getMasterVolume() + 0.10f);
            sm.setMasterVolume(newVol);
            updateMenuOptionTexts();
            sm.playSound("coin");
        }
    }

    if (input.isActionJustPressed(Action::MenuConfirm) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
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

            updateMenuOptionTexts();
            SoundManager::getInstance().playSound("coin");
        }
        else if (selectedIndex == 2) {
            // Level Editor
            GameEngine::getInstance().getStateManager().pushState(new LevelEditorState());
        }
        else if (selectedIndex == 3) {
            // Audio Settings State
            GameEngine::getInstance().getStateManager().pushState(new AudioSettingsState());
        }
        else if (selectedIndex == 4) {
            // Custom Key Settings
            GameEngine::getInstance().getStateManager().pushState(new SettingsState());
        } 
        else if (selectedIndex == 5) {
            // Login
            GameEngine::getInstance().getStateManager().pushState(new LoginState());
        } 
        else if (selectedIndex == 6) {
            // Register
            GameEngine::getInstance().getStateManager().pushState(new RegisterState());
        } 
        else if (selectedIndex == 7) {
            // Quit
            GameEngine::getInstance().exitGame();
        }
    }
}

void MainMenuState::update(float dt) {
    animTime += dt;
    updateMenuOptionTexts();
}

void MainMenuState::draw() {
    // 1. Sky background
    ClearBackground(Color{ 80, 136, 160, 255 });

    AssetManager& assets = AssetManager::getInstance();

    // 2. Distant clouds background from background.png (18,18) -> (3089,256)
    Texture2D bgTex = assets.getTexture("background");
    if (bgTex.id != 0) {
        Rectangle srcBg = { 18.0f, 18.0f, 400.0f, 238.0f };
        float destHeight = 238.0f * 2.0f; // 476.0f
        float destY = (float)GetScreenHeight() - destHeight - 64.0f; // Aligned atop ground
        DrawTexturePro(bgTex, srcBg, Rectangle{ 0.0f, destY, (float)GetScreenWidth(), destHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // 3. Clouds from nobackgroundcloud.png: (388,16) -> (496,92) and (830,16) -> (913,82)
    Texture2D cloudTex = assets.getTexture("nobackgroundcloud");
    if (cloudTex.id != 0) {
        Rectangle srcCloud1 = { 388.0f, 16.0f, 108.0f, 76.0f };
        Rectangle srcCloud2 = { 830.0f, 16.0f, 83.0f, 66.0f };

        float c1X = fmod(30.0f + animTime * 12.0f, (float)(GetScreenWidth() + 250)) - 220.0f;
        float c2X = fmod(540.0f + animTime * 8.0f, (float)(GetScreenWidth() + 200)) - 180.0f;

        DrawTexturePro(cloudTex, srcCloud1, Rectangle{ c1X, 25.0f, 216.0f, 152.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        DrawTexturePro(cloudTex, srcCloud2, Rectangle{ c2X, 45.0f, 166.0f, 132.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // 4. Ground floor (#) from overworld1.png: (0,208) -> (1103,239)
    Texture2D worldTex = assets.getTexture("world");
    const float groundY = 536.0f;
    const float groundH = 64.0f;
    if (worldTex.id != 0) {
        int tileWidth = 32;
        int totalCols = (GetScreenWidth() + tileWidth - 1) / tileWidth;
        for (int col = 0; col < totalCols; ++col) {
            float srcX = (float)((col * 16) % 1104);
            Rectangle srcGround = { srcX, 208.0f, 16.0f, 32.0f };
            Rectangle destGround = { (float)(col * tileWidth), groundY, (float)tileWidth, groundH };
            DrawTexturePro(worldTex, srcGround, destGround, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
    } else {
        DrawRectangle(0, (int)groundY, GetScreenWidth(), (int)groundH, Color{ 148, 76, 36, 255 });
        DrawRectangle(0, (int)groundY, GetScreenWidth(), 8, Color{ 56, 164, 40, 255 });
    }

    // 5. Castle from overworld1.png: (3233,128) -> (3326,207)
    if (worldTex.id != 0) {
        Rectangle srcCastle = { 3233.0f, 128.0f, 94.0f, 80.0f };
        float castleScale = 2.0f;
        float castleW = srcCastle.width * castleScale;   // 188.0f
        float castleH = srcCastle.height * castleScale;  // 160.0f
        float castleX = (float)GetScreenWidth() - castleW - 40.0f; // 572.0f
        float castleY = groundY - castleH;               // 376.0f
        DrawTexturePro(worldTex, srcCastle, Rectangle{ castleX, castleY, castleW, castleH }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // 6. Super Mario Board Title from mainmenu.png: (233,28) -> (424,115)
    Texture2D menuTex = assets.getTexture("mainmenu");
    if (menuTex.id != 0) {
        Rectangle source = { 233.0f, 28.0f, 192.0f, 88.0f };
        float scale = 2.0f;
        float destWidth = source.width * scale;
        float destHeight = source.height * scale;
        float destX = (GetScreenWidth() - destWidth) / 2.0f;
        float destY = 20.0f;
        DrawTexturePro(menuTex, source, Rectangle{ destX, destY, destWidth, destHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawText("SUPER MARIO OOP", 150, 35, 46, MAROON);
        DrawText("C++ & Raylib Project", 250, 85, 20, DARKGRAY);
    }

    // 7. Menu Options Box & Text
    float menuBoxX = 65.0f;
    float menuBoxY = 200.0f;
    float menuBoxW = 420.0f;
    float optionSpacing = 37.0f;
    float menuBoxH = (float)mainOptions.size() * optionSpacing + 14.0f;

    // Elegant semi-transparent frosted card backdrop
    DrawRectangleRounded(Rectangle{ menuBoxX + 4.0f, menuBoxY + 4.0f, menuBoxW, menuBoxH }, 0.07f, 8, Fade(BLACK, 0.45f));
    DrawRectangleRounded(Rectangle{ menuBoxX, menuBoxY, menuBoxW, menuBoxH }, 0.07f, 8, Fade(BLACK, 0.60f));
    DrawRectangleRoundedLines(Rectangle{ menuBoxX, menuBoxY, menuBoxW, menuBoxH }, 0.07f, 8, 2.0f, Fade(GOLD, 0.55f));

    for (size_t i = 0; i < mainOptions.size(); ++i) {
        bool selected = (i == (size_t)selectedIndex);
        Color textColor = selected ? YELLOW : WHITE;
        std::string prefix = selected ? "> " : "  ";
        std::string text = prefix + mainOptions[i];
        
        int textY = (int)(menuBoxY + 10.0f + (float)i * optionSpacing);

        if (selected) {
            DrawRectangleRounded(Rectangle{ menuBoxX + 8.0f, (float)textY - 2.0f, menuBoxW - 16.0f, 28.0f }, 0.3f, 4, Fade(GOLD, 0.15f));
        }

        // Subtle drop shadow for crisp readability
        DrawText(text.c_str(), (int)(menuBoxX + 22.0f), textY + 2, 20, Fade(BLACK, 0.8f));
        DrawText(text.c_str(), (int)(menuBoxX + 20.0f), textY, 20, textColor);
    }

    // 8. Navigation Hint at the bottom on the ground
    std::string hint = "Use UP/DOWN to navigate  |  ENTER to select";
    if (selectedIndex == 3) {
        hint = "UP/DOWN: Navigate  |  LEFT/RIGHT: Quick Volume  |  ENTER: Audio Settings";
    }
    int hintWidth = MeasureText(hint.c_str(), 17);
    int hintX = (GetScreenWidth() - hintWidth) / 2;
    DrawText(hint.c_str(), hintX + 1, (int)(groundY + 22.0f) + 1, 17, Fade(BLACK, 0.8f));
    DrawText(hint.c_str(), hintX, (int)(groundY + 22.0f), 17, WHITE);
}

void MainMenuState::onBack() {
    // No-op: Menu has a Quit button, ESC does nothing
}
