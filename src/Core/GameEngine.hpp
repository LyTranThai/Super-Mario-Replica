#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "GameStateManager.hpp"
#include "InputManager.hpp"
#include "Persistence/Account.hpp"

class GameEngine {
private:
    GameStateManager stateManager;
    InputManager inputManager;
    Account activeAccount;
    bool isRunning;
    bool isInitialized;

    GameEngine();
    void disableWindowIME();

public:
    static GameEngine& getInstance() {
        static GameEngine instance;
        return instance;
    }

    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    ~GameEngine();

    void init();
    void run();
    void cleanup();

    GameStateManager& getStateManager() { return stateManager; }
    InputManager& getInputManager() { return inputManager; }
    Account& getActiveAccount() { return activeAccount; }
    void setActiveAccount(const Account& acc) { activeAccount = acc; }
    void exitGame() { isRunning = false; }
};

#endif // GAME_ENGINE_H
