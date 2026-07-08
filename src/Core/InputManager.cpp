#include "InputManager.h"
#include "raylib.h"

InputManager::InputManager() {
    // Set up default action bindings
    bindKey(KEY_A, Action::MoveLeft);
    bindKey(KEY_D, Action::MoveRight);
    bindKey(KEY_W, Action::Jump);       // W for jump
    bindKey(KEY_S, Action::Crouch);     // S for crouch
    bindKey(KEY_LEFT_SHIFT, Action::Run);
    bindKey(KEY_J, Action::Shoot);
    bindKey(KEY_P, Action::Pause);

    // Menu switch keys default to arrow keys to avoid sharing W/S initially
    bindKey(KEY_UP, Action::MenuUp);
    bindKey(KEY_DOWN, Action::MenuDown);
    bindKey(KEY_ENTER, Action::MenuConfirm);
}

bool InputManager::bindKey(int key, Action action) {
    // Check if this key is already bound to any OTHER action
    for (auto const& binding : bindings) {
        if (binding.second == key && binding.first != action) {
            return false; // Key exists for another action, reject!
        }
    }

    // Find the first binding for this action and update it
    bool found = false;
    for (auto& binding : bindings) {
        if (binding.first == action) {
            binding.second = key;
            found = true;
            break;
        }
    }
    if (!found) {
        bindings.push_back({action, key});
    }
    return true;
}

void InputManager::update() {
    // No-op: Native Raylib queries are used directly in action checkers
}

bool InputManager::isActionPressed(Action action) const {
    for (auto const& binding : bindings) {
        if (binding.first == action) {
            if (IsKeyDown(binding.second)) {
                return true;
            }
        }
    }
    return false;
}

bool InputManager::isActionJustPressed(Action action) const {
    for (auto const& binding : bindings) {
        if (binding.first == action) {
            if (IsKeyPressed(binding.second)) {
                return true;
            }
        }
    }
    return false;
}

bool InputManager::isActionReleased(Action action) const {
    for (auto const& binding : bindings) {
        if (binding.first == action) {
            if (IsKeyReleased(binding.second)) {
                return true;
            }
        }
    }
    return false;
}

int InputManager::getBoundKey(Action action) const {
    for (auto const& binding : bindings) {
        if (binding.first == action) {
            return binding.second;
        }
    }
    return 0; // Return 0 (KEY_NULL) if not bound
}
