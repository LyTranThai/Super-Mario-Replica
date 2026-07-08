#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include "Core/GameState.h"
#include "Core/EventSystem.h"
#include <memory>
#include <string>

class Level;

class GameplayState : public GameState, public EventListener {
private:
    std::unique_ptr<Level> level;
    int currentLevelIndex;
    std::vector<std::string> levelFiles;

    bool pendingReset = false;
    bool pendingGameOver = false;
    bool pendingWin = false;
    int pendingScore = 0;
    int pendingLives = 3;

    void loadLevel(int index);

public:
    GameplayState();
    GameplayState(int startLevelIndex);
    ~GameplayState() override;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;

    // Observer callback
    void onEvent(EventType type, void* data = nullptr) override;
};

#endif // GAMEPLAY_STATE_H
