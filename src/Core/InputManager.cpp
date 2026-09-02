#include "InputManager.h"
#include "raylib.h"

InputManager::InputManager() {
    bindingsPerPlayer.resize(2);

    // Set up default action bindings for Player 1 (Left side of keyboard / WASD)
    bindKey(KEY_A, Action::MoveLeft, 0);
    bindKey(KEY_D, Action::MoveRight, 0);
    bindKey(KEY_W, Action::Jump, 0);       // W for jump
    bindKey(KEY_S, Action::Crouch, 0);     // S for crouch
    bindKey(KEY_LEFT_SHIFT, Action::Run, 0);
    bindKey(KEY_J, Action::Shoot, 0);
    addKeyBinding(KEY_F, Action::Shoot, 0); // Alternate shoot key
    bindKey(KEY_P, Action::Pause, 0);

    // Menu switch keys default to arrow keys to avoid sharing W/S initially
    bindKey(KEY_UP, Action::MenuUp, 0);
    bindKey(KEY_DOWN, Action::MenuDown, 0);
    bindKey(KEY_ENTER, Action::MenuConfirm, 0);

    // Set up default action bindings for Player 2 (Right side of keyboard / Arrows)
    bindKey(KEY_LEFT, Action::MoveLeft, 1);
    bindKey(KEY_RIGHT, Action::MoveRight, 1);
    bindKey(KEY_UP, Action::Jump, 1);      // UP for jump
    bindKey(KEY_DOWN, Action::Crouch, 1);  // DOWN for crouch
    bindKey(KEY_RIGHT_SHIFT, Action::Run, 1);
    bindKey(KEY_L, Action::Shoot, 1);      // L for shoot
    addKeyBinding(KEY_SLASH, Action::Shoot, 1); // Alternate shoot key
    addKeyBinding(KEY_ENTER, Action::Shoot, 1); // Alternate shoot key
    bindKey(KEY_P, Action::Pause, 1);
}

std::vector<std::pair<Action, int>>& InputManager::getPlayerBindings(int playerIndex) {
    if (playerIndex < 0) playerIndex = 0;
    if (playerIndex >= (int)bindingsPerPlayer.size()) {
        bindingsPerPlayer.resize(playerIndex + 1);
    }
    return bindingsPerPlayer[playerIndex];
}

const std::vector<std::pair<Action, int>>& InputManager::getPlayerBindings(int playerIndex) const {
    static const std::vector<std::pair<Action, int>> emptyBindings;
    if (playerIndex < 0 || playerIndex >= (int)bindingsPerPlayer.size()) {
        return emptyBindings;
    }
    return bindingsPerPlayer[playerIndex];
}

bool InputManager::bindKey(int key, Action action, int playerIndex) {
    auto& list = getPlayerBindings(playerIndex);
    // Check if this key is already bound to any OTHER action for this player
    for (auto const& binding : list) {
        if (binding.second == key && binding.first != action) {
            return false; // Key exists for another action, reject!
        }
    }

    // Find the first binding for this action and update it
    for (auto& binding : list) {
        if (binding.first == action) {
            binding.second = key;
            return true;
        }
    }
    list.push_back({action, key});
    return true;
}

bool InputManager::addKeyBinding(int key, Action action, int playerIndex) {
    auto& list = getPlayerBindings(playerIndex);
    for (auto const& binding : list) {
        if (binding.second == key && binding.first != action) {
            return false; // Key exists for another action, reject!
        }
        if (binding.second == key && binding.first == action) {
            return true; // Already bound to this action
        }
    }
    list.push_back({action, key});
    return true;
}

void InputManager::update() {
    // No-op: Native Raylib queries are used directly in action checkers
}

bool InputManager::isActionPressed(Action action, int playerIndex) const {
    const auto& list = getPlayerBindings(playerIndex);
    for (auto const& binding : list) {
        if (binding.first == action) {
            if (IsKeyDown(binding.second)) {
                return true;
            }
        }
    }
    return false;
}

bool InputManager::isActionJustPressed(Action action, int playerIndex) const {
    const auto& list = getPlayerBindings(playerIndex);
    for (auto const& binding : list) {
        if (binding.first == action) {
            if (IsKeyPressed(binding.second)) {
                return true;
            }
        }
    }
    return false;
}

bool InputManager::isActionReleased(Action action, int playerIndex) const {
    const auto& list = getPlayerBindings(playerIndex);
    for (auto const& binding : list) {
        if (binding.first == action) {
            if (IsKeyReleased(binding.second)) {
                return true;
            }
        }
    }
    return false;
}

int InputManager::getBoundKey(Action action, int playerIndex) const {
    const auto& list = getPlayerBindings(playerIndex);
    for (auto const& binding : list) {
        if (binding.first == action) {
            return binding.second;
        }
    }
    return 0; // Return 0 (KEY_NULL) if not bound
}
