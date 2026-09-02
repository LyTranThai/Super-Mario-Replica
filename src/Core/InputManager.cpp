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
    bindKey(KEY_P, Action::Pause, 1);
}

bool InputManager::bindKey(int key, Action action, int playerIndex) {
    auto& list = (playerIndex == 1) ? bindingsPerPlayer[1] : bindingsPerPlayer[0];
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

void InputManager::update() {
    // No-op: Native Raylib queries are used directly in action checkers
}

bool InputManager::isActionPressed(Action action, int playerIndex) const {
    const auto& list = (playerIndex == 1) ? bindingsPerPlayer[1] : bindingsPerPlayer[0];
    for (auto const& binding : list) {
        if (binding.first == action) {
            if (IsKeyDown(binding.second)) {
                return true;
            }
        }
    }
    // Alternative keys for comfortable co-op
    if (action == Action::Shoot) {
        if (playerIndex == 0 && IsKeyDown(KEY_F)) return true;
        if (playerIndex == 1 && (IsKeyDown(KEY_SLASH) || IsKeyDown(KEY_ENTER))) return true;
    }
    return false;
}

bool InputManager::isActionJustPressed(Action action, int playerIndex) const {
    const auto& list = (playerIndex == 1) ? bindingsPerPlayer[1] : bindingsPerPlayer[0];
    for (auto const& binding : list) {
        if (binding.first == action) {
            if (IsKeyPressed(binding.second)) {
                return true;
            }
        }
    }
    if (action == Action::Shoot) {
        if (playerIndex == 0 && IsKeyPressed(KEY_F)) return true;
        if (playerIndex == 1 && (IsKeyPressed(KEY_SLASH) || IsKeyPressed(KEY_ENTER))) return true;
    }
    return false;
}

bool InputManager::isActionReleased(Action action, int playerIndex) const {
    const auto& list = (playerIndex == 1) ? bindingsPerPlayer[1] : bindingsPerPlayer[0];
    for (auto const& binding : list) {
        if (binding.first == action) {
            if (IsKeyReleased(binding.second)) {
                return true;
            }
        }
    }
    if (action == Action::Shoot) {
        if (playerIndex == 0 && IsKeyReleased(KEY_F)) return true;
        if (playerIndex == 1 && (IsKeyReleased(KEY_SLASH) || IsKeyReleased(KEY_ENTER))) return true;
    }
    return false;
}

int InputManager::getBoundKey(Action action, int playerIndex) const {
    const auto& list = (playerIndex == 1) ? bindingsPerPlayer[1] : bindingsPerPlayer[0];
    for (auto const& binding : list) {
        if (binding.first == action) {
            return binding.second;
        }
    }
    return 0; // Return 0 (KEY_NULL) if not bound
}
