#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "Core/GameState.h"

class GameOverState : public GameState {
private:
    bool isWin;
    int score;

public:
    GameOverState(bool win, int finalScore);
    ~GameOverState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // GAME_OVER_STATE_H
