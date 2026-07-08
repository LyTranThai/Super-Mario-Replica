#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <vector>

class GameState;
class InputManager;

enum class StateActionType {
    PUSH,
    POP,
    CHANGE,
    CLEAR
};

struct StateAction {
    StateActionType type;
    GameState* state;
};

class GameStateManager {
private:
    std::vector<GameState*> states; // Using vector as a stack to allow bottom-to-top rendering
    std::vector<StateAction> pendingActions;

public:
    GameStateManager() = default;
    ~GameStateManager();

    void pushState(GameState* state);
    void popState();
    void changeState(GameState* state);

    void processPendingActions();

    void handleInput(const InputManager& input);
    void update(float dt);
    void draw();

    bool isEmpty() const { return states.empty(); }
    void clear();
};

#endif // GAME_STATE_MANAGER_H
