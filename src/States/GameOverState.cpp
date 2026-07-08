#include "GameOverState.hpp"
#include "MainMenuState.hpp"
#include "Core/GameEngine.hpp"
#include "Core/SoundManager.hpp"
#include "raylib.h"

GameOverState::GameOverState(bool win, int finalScore) : isWin(win), score(finalScore) {}

void GameOverState::init() {
    SoundManager::getInstance().stopMusic();
    // Play win/die music stream or sound effect
    if (isWin) {
        SoundManager::getInstance().playSound("coin"); // Placeholder success chime
    } else {
        SoundManager::getInstance().playSound("die"); // Death sound
    }
}

void GameOverState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::MenuConfirm)) {
        GameEngine::getInstance().getStateManager().popState();
    }
}

void GameOverState::update(float dt) {
    (void)dt;
}

void GameOverState::onBack() {
    GameEngine::getInstance().getStateManager().popState();
}

void GameOverState::draw() {
    ClearBackground(BLACK);

    if (isWin) {
        DrawText("VICTORY!", 280, 150, 50, GOLD);
        DrawText("Congratulations! You have saved the Mushroom Kingdom!", 120, 240, 20, WHITE);
    } else {
        DrawText("GAME OVER", 260, 150, 50, RED);
        DrawText("You ran out of lives!", 300, 240, 20, WHITE);
    }

    std::string scoreMsg = "Final Score: " + std::to_string(score);
    DrawText(scoreMsg.c_str(), 320, 320, 25, GREEN);

    DrawText("Press [ENTER] or [ESC] to return to Level Select", 170, 420, 20, LIGHTGRAY);
}
