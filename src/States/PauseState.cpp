#include "PauseState.h"
#include "MainMenuState.h"
#include "Core/GameEngine.h"
#include "raylib.h"

PauseState::PauseState() : selectedIndex(0) {}

void PauseState::init() {
    options = { "CONTINUE", "QUIT TO LEVEL SELECT" };
}

void PauseState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::MenuUp) || IsKeyPressed(KEY_UP)) {
        selectedIndex = (selectedIndex - 1 + options.size()) % options.size();
    }
    if (input.isActionJustPressed(Action::MenuDown) || IsKeyPressed(KEY_DOWN)) {
        selectedIndex = (selectedIndex + 1) % options.size();
    }
    if (input.isActionJustPressed(Action::MenuConfirm)) {
        if (selectedIndex == 0) {
            GameEngine::getInstance().getStateManager().popState();
        } else if (selectedIndex == 1) {
            GameStateManager& gsm = GameEngine::getInstance().getStateManager();
            gsm.popState();
            gsm.popState();
        }
    }
}

void PauseState::update(float dt) {
    (void)dt;
}

void PauseState::onBack() {
    GameEngine::getInstance().getStateManager().popState();
}

void PauseState::draw() {
    // Solid background (black) instead of transparent overlay
    ClearBackground(BLACK);
    
    // Centered smaller window
    int boxW = 400;
    int boxH = 260;
    int boxX = (GetScreenWidth() - boxW) / 2; 
    int boxY = (GetScreenHeight() - boxH) / 2; 

    // Drop shadow
    DrawRectangle(boxX + 6, boxY + 6, boxW, boxH, Fade(BLACK, 0.5f));

    // Modal Box
    DrawRectangle(boxX, boxY, boxW, boxH, DARKGRAY);
    DrawRectangleLinesEx(Rectangle{(float)boxX, (float)boxY, (float)boxW, (float)boxH}, 3.0f, RED);

    // Dialog contents
    DrawText("GAME PAUSED", boxX + 95, boxY + 30, 30, GOLD);

    for (size_t i = 0; i < options.size(); ++i) {
        Color color = (i == (size_t)selectedIndex) ? RED : WHITE;
        std::string prefix = (i == (size_t)selectedIndex) ? "> " : "  ";
        DrawText((prefix + options[i]).c_str(), boxX + 60, boxY + 110 + i * 40, 20, color);
    }

    DrawText("Press [ESC] or choose CONTINUE to resume", boxX + 30, boxY + 210, 16, LIGHTGRAY);
}
