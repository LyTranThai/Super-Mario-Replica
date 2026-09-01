#include "AudioSettingsState.h"
#include "raylib.h"
#include "Core/GameEngine.h"
#include "Core/SoundManager.h"
#include "Core/AssetManager.h"
#include <cmath>
#include <algorithm>
#include <iostream>

AudioSettingsState::AudioSettingsState()
    : selectedIndex(0), animTime(0.0f), currentTestSoundIndex(0) {}

void AudioSettingsState::init() {
    options = {
        "MASTER VOLUME",
        "MUSIC VOLUME",
        "SFX VOLUME",
        "MUTE AUDIO",
        "TEST SOUND FX",
        "RETURN TO PREVIOUS"
    };

    testSoundIds = {
        "coin",
        "jump",
        "power_up",
        "fireball",
        "break",
        "pipe_warp",
        "stomp",
        "koopa_kicked"
    };
    currentTestSoundIndex = 0;
    animTime = 0.0f;
}

void AudioSettingsState::adjustVolume(int direction) {
    SoundManager& sm = SoundManager::getInstance();

    if (selectedIndex == 0) {
        // Master Volume
        if (sm.isMuted() && direction > 0) {
            sm.setMuted(false);
        }
        float newVol = std::clamp(sm.getMasterVolume() + (float)direction * 0.10f, 0.0f, 1.0f);
        sm.setMasterVolume(newVol);
        if (direction != 0) {
            sm.playSound("coin");
        }
    }
    else if (selectedIndex == 1) {
        // Music Volume
        float newVol = std::clamp(sm.getMusicVolume() + (float)direction * 0.10f, 0.0f, 1.0f);
        sm.setMusicVolume(newVol);
    }
    else if (selectedIndex == 2) {
        // SFX Volume
        float newVol = std::clamp(sm.getSoundVolume() + (float)direction * 0.10f, 0.0f, 1.0f);
        sm.setSoundVolume(newVol);
        if (direction != 0 && !testSoundIds.empty()) {
            sm.playSound(testSoundIds[currentTestSoundIndex]);
        }
    }
    else if (selectedIndex == 3) {
        // Mute toggle
        sm.toggleMute();
    }
    else if (selectedIndex == 4) {
        // Cycle test sounds
        int total = (int)testSoundIds.size();
        currentTestSoundIndex = (currentTestSoundIndex + direction + total) % total;
        sm.playSound(testSoundIds[currentTestSoundIndex]);
    }
}

void AudioSettingsState::triggerAction() {
    SoundManager& sm = SoundManager::getInstance();

    if (selectedIndex >= 0 && selectedIndex <= 2) {
        // Step forward or loop volume
        float currentVol = (selectedIndex == 0) ? sm.getMasterVolume() :
                           (selectedIndex == 1) ? sm.getMusicVolume() : sm.getSoundVolume();
        float nextVol = currentVol + 0.10f;
        if (nextVol > 1.01f) nextVol = 0.0f;
        
        if (selectedIndex == 0) sm.setMasterVolume(nextVol);
        else if (selectedIndex == 1) sm.setMusicVolume(nextVol);
        else if (selectedIndex == 2) sm.setSoundVolume(nextVol);

        sm.playSound("coin");
    }
    else if (selectedIndex == 3) {
        sm.toggleMute();
    }
    else if (selectedIndex == 4) {
        if (!testSoundIds.empty()) {
            sm.playSound(testSoundIds[currentTestSoundIndex]);
        }
    }
    else if (selectedIndex == 5) {
        onBack();
    }
}

void AudioSettingsState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::MenuUp) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + (int)options.size()) % (int)options.size();
    }
    if (input.isActionJustPressed(Action::MenuDown) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % (int)options.size();
    }

    if (input.isActionJustPressed(Action::MoveLeft) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        adjustVolume(-1);
    }
    if (input.isActionJustPressed(Action::MoveRight) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        adjustVolume(1);
    }

    if (input.isActionJustPressed(Action::MenuConfirm) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        triggerAction();
    }

    // Mouse interactions
    Vector2 mousePos = GetMousePosition();
    float cardX = 120.0f;
    float cardY = 85.0f;
    float cardW = 560.0f;
    float rowStartY = cardY + 80.0f;
    float rowSpacing = 46.0f;

    for (size_t i = 0; i < options.size(); ++i) {
        float rY = rowStartY + (float)i * rowSpacing;
        Rectangle rowRect = { cardX + 15.0f, rY - 4.0f, cardW - 30.0f, 38.0f };

        if (CheckCollisionPointRec(mousePos, rowRect)) {
            selectedIndex = (int)i;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (i <= 2) {
                    // Check if clicked near left arrow, right arrow, or slider bar
                    float sliderX = cardX + 280.0f;
                    float sliderW = 150.0f;
                    Rectangle leftArrowRect = { sliderX - 30.0f, rY, 24.0f, 24.0f };
                    Rectangle rightArrowRect = { sliderX + sliderW + 6.0f, rY, 24.0f, 24.0f };
                    Rectangle barRect = { sliderX, rY + 3.0f, sliderW, 18.0f };

                    if (CheckCollisionPointRec(mousePos, leftArrowRect)) {
                        adjustVolume(-1);
                    } else if (CheckCollisionPointRec(mousePos, rightArrowRect)) {
                        adjustVolume(1);
                    } else if (CheckCollisionPointRec(mousePos, barRect)) {
                        float fraction = (mousePos.x - sliderX) / sliderW;
                        fraction = std::clamp(fraction, 0.0f, 1.0f);
                        // Round to nearest 5%
                        fraction = std::round(fraction * 20.0f) / 20.0f;
                        SoundManager& sm = SoundManager::getInstance();
                        if (i == 0) sm.setMasterVolume(fraction);
                        else if (i == 1) sm.setMusicVolume(fraction);
                        else if (i == 2) sm.setSoundVolume(fraction);
                        sm.playSound("coin");
                    } else {
                        triggerAction();
                    }
                } else {
                    triggerAction();
                }
            }
        }
    }
}

void AudioSettingsState::update(float dt) {
    animTime += dt;
}

void AudioSettingsState::draw() {
    // 1. Sky backdrop
    ClearBackground(Color{ 80, 136, 160, 255 });

    AssetManager& assets = AssetManager::getInstance();

    // 2. Distant clouds background
    Texture2D bgTex = assets.getTexture("background");
    if (bgTex.id != 0) {
        Rectangle srcBg = { 18.0f, 18.0f, 400.0f, 238.0f };
        float destHeight = 238.0f * 2.0f;
        float destY = (float)GetScreenHeight() - destHeight - 64.0f;
        DrawTexturePro(bgTex, srcBg, Rectangle{ 0.0f, destY, (float)GetScreenWidth(), destHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // 3. Floating clouds
    Texture2D cloudTex = assets.getTexture("nobackgroundcloud");
    if (cloudTex.id != 0) {
        Rectangle srcCloud1 = { 388.0f, 16.0f, 108.0f, 76.0f };
        Rectangle srcCloud2 = { 830.0f, 16.0f, 83.0f, 66.0f };

        float c1X = fmod(30.0f + animTime * 12.0f, (float)(GetScreenWidth() + 250)) - 220.0f;
        float c2X = fmod(540.0f + animTime * 8.0f, (float)(GetScreenWidth() + 200)) - 180.0f;

        DrawTexturePro(cloudTex, srcCloud1, Rectangle{ c1X, 25.0f, 216.0f, 152.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, Fade(WHITE, 0.8f));
        DrawTexturePro(cloudTex, srcCloud2, Rectangle{ c2X, 45.0f, 166.0f, 132.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, Fade(WHITE, 0.8f));
    }

    // 4. Ground floor
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
    }

    // 5. Main Card Container
    float cardW = 560.0f;
    float cardH = 410.0f;
    float cardX = ((float)GetScreenWidth() - cardW) / 2.0f;
    float cardY = 75.0f;

    // Drop shadow
    DrawRectangleRounded(Rectangle{ cardX + 6.0f, cardY + 6.0f, cardW, cardH }, 0.06f, 8, Fade(BLACK, 0.45f));
    // Frosted card
    DrawRectangleRounded(Rectangle{ cardX, cardY, cardW, cardH }, 0.06f, 8, Fade(BLACK, 0.72f));
    // Gold border
    DrawRectangleRoundedLines(Rectangle{ cardX, cardY, cardW, cardH }, 0.06f, 8, 2.5f, Fade(GOLD, 0.8f));

    // Card Header
    const char* title = "AUDIO SETTINGS";
    int titleW = MeasureText(title, 28);
    int titleX = (int)(cardX + (cardW - (float)titleW) / 2.0f);
    DrawText(title, titleX + 2, (int)(cardY + 20.0f) + 2, 28, Fade(BLACK, 0.8f));
    DrawText(title, titleX, (int)(cardY + 20.0f), 28, GOLD);

    const char* subtitle = "Configure Master, Music, and Sound Effects volume";
    int subW = MeasureText(subtitle, 16);
    int subX = (int)(cardX + (cardW - (float)subW) / 2.0f);
    DrawText(subtitle, subX, (int)(cardY + 54.0f), 16, LIGHTGRAY);

    DrawLineEx(Vector2{ cardX + 30.0f, cardY + 76.0f }, Vector2{ cardX + cardW - 30.0f, cardY + 76.0f }, 1.0f, Fade(GOLD, 0.35f));

    SoundManager& sm = SoundManager::getInstance();

    float rowStartY = cardY + 92.0f;
    float rowSpacing = 46.0f;

    for (size_t i = 0; i < options.size(); ++i) {
        bool isSelected = (selectedIndex == (int)i);
        float rY = rowStartY + (float)i * rowSpacing;

        // Highlight pill for selected option
        if (isSelected) {
            Rectangle selPill = { cardX + 20.0f, rY - 4.0f, cardW - 40.0f, 36.0f };
            DrawRectangleRounded(selPill, 0.3f, 4, Fade(GOLD, 0.18f));
            DrawRectangleRoundedLines(selPill, 0.3f, 4, 1.5f, Fade(YELLOW, 0.6f));
        }

        // Row prefix & label
        std::string prefix = isSelected ? "> " : "  ";
        std::string label = prefix + options[i];
        Color labelColor = isSelected ? YELLOW : WHITE;

        DrawText(label.c_str(), (int)(cardX + 32.0f), (int)rY + 2, 20, Fade(BLACK, 0.8f));
        DrawText(label.c_str(), (int)(cardX + 30.0f), (int)rY, 20, labelColor);

        // Value controls on the right side
        if (i <= 2) {
            // Volume Sliders (Master, Music, SFX)
            float vol = (i == 0) ? sm.getMasterVolume() :
                        (i == 1) ? sm.getMusicVolume() : sm.getSoundVolume();
            int percent = (int)std::round(vol * 100.0f);

            float sliderX = cardX + 310.0f;
            float sliderW = 120.0f;
            float sliderH = 14.0f;
            float sliderY = rY + 4.0f;

            // Left Arrow
            DrawText("<", (int)(sliderX - 22.0f), (int)rY - 1, 22, isSelected ? YELLOW : LIGHTGRAY);

            // Slider track (empty background)
            DrawRectangleRounded(Rectangle{ sliderX, sliderY, sliderW, sliderH }, 0.4f, 4, Color{ 40, 40, 40, 255 });
            DrawRectangleRoundedLines(Rectangle{ sliderX, sliderY, sliderW, sliderH }, 0.4f, 4, 1.0f, Fade(GRAY, 0.7f));

            // Filled portion
            if (percent > 0) {
                float fillW = std::max(6.0f, sliderW * vol);
                Color barColor = (i == 0) ? GOLD : (i == 1) ? SKYBLUE : GREEN;
                if (sm.isMuted() && i == 0) {
                    barColor = GRAY;
                }
                DrawRectangleRounded(Rectangle{ sliderX, sliderY, fillW, sliderH }, 0.4f, 4, barColor);
            }

            // Right Arrow
            DrawText(">", (int)(sliderX + sliderW + 10.0f), (int)rY - 1, 22, isSelected ? YELLOW : LIGHTGRAY);

            // Percentage Text
            std::string pctText = std::to_string(percent) + "%";
            if (i == 0 && sm.isMuted()) {
                pctText = "MUTED";
            }
            DrawText(pctText.c_str(), (int)(sliderX + sliderW + 32.0f), (int)rY + 1, 18, isSelected ? YELLOW : WHITE);
        }
        else if (i == 3) {
            // Mute Toggle
            bool muted = sm.isMuted();
            const char* statusText = muted ? "[ MUTED ]" : "[ UNMUTED ]";
            Color statusColor = muted ? RED : LIME;
            DrawText(statusText, (int)(cardX + 340.0f), (int)rY, 20, statusColor);
        }
        else if (i == 4) {
            // Test Sound FX
            std::string sndName = testSoundIds.empty() ? "N/A" : testSoundIds[currentTestSoundIndex];
            // Format sound name uppercase
            std::string btnText = "[ " + sndName + " > PLAY ]";
            DrawText(btnText.c_str(), (int)(cardX + 280.0f), (int)rY, 18, isSelected ? GOLD : SKYBLUE);
        }
        else if (i == 5) {
            // Return to previous menu
            DrawText("[ PRESS ENTER OR ESC ]", (int)(cardX + 300.0f), (int)rY, 18, isSelected ? YELLOW : LIGHTGRAY);
        }
    }

    // Bottom Navigation Hint
    const char* hint = "UP/DOWN: Navigate  |  LEFT/RIGHT: Adjust Volume  |  ENTER: Select  |  ESC: Back";
    int hintW = MeasureText(hint, 17);
    int hintX = (GetScreenWidth() - hintW) / 2;
    DrawText(hint, hintX + 1, (int)(groundY + 22.0f) + 1, 17, Fade(BLACK, 0.8f));
    DrawText(hint, hintX, (int)(groundY + 22.0f), 17, WHITE);
}

void AudioSettingsState::onBack() {
    GameEngine::getInstance().getStateManager().popState();
}
