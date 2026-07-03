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
    std::vector<std::pair<Action, int>> bindings;
    std::map<Action, bool> actionStates;
    std::map<Action, bool> prevActionStates;

public:
    InputManager();

    bool bindKey(int key, Action action);
    void update(); // Queries Raylib input states

    bool isActionPressed(Action action) const;
    bool isActionJustPressed(Action action) const;
    bool isActionReleased(Action action) const;

    // Helper to get raw key code currently bound to an action
    int getBoundKey(Action action) const;
};

#endif // INPUT_MANAGER_H
