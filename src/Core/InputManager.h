#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <map>
#include <vector>
#include <utility>

enum class Action {
    MoveLeft,
    MoveRight,
    Jump,
    Crouch,
    Run, // Shift key
    Shoot, // Shoot fireballs / kick / grab
    Pause,
    MenuUp,
    MenuDown,
    MenuConfirm
};

class InputManager {
private:
    std::vector<std::vector<std::pair<Action, int>>> bindingsPerPlayer;
    std::map<Action, bool> actionStates;
    std::map<Action, bool> prevActionStates;

    std::vector<std::pair<Action, int>>& getPlayerBindings(int playerIndex);
    const std::vector<std::pair<Action, int>>& getPlayerBindings(int playerIndex) const;

public:
    InputManager();

    bool bindKey(int key, Action action, int playerIndex = 0);
    bool addKeyBinding(int key, Action action, int playerIndex = 0);
    void update(); // Queries Raylib input states

    bool isActionPressed(Action action, int playerIndex = 0) const;
    bool isActionJustPressed(Action action, int playerIndex = 0) const;
    bool isActionReleased(Action action, int playerIndex = 0) const;

    // Helper to get raw key code currently bound to an action
    int getBoundKey(Action action, int playerIndex = 0) const;
};

#endif // INPUT_MANAGER_H
