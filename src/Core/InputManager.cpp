#include "InputManager.h"
#include "raylib.h"

InputManager::InputManager() {
    bindingsPerPlayer.resize(2);
    actionStates.resize(2);
    prevActionStates.resize(2);

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

    // Set up default gamepad bindings for players
    gamepadBindingsPerPlayer.resize(2);
    for (int p = 0; p < 2; ++p) {
        bindGamepadButton(GAMEPAD_BUTTON_LEFT_FACE_LEFT, Action::MoveLeft, p);
        bindGamepadButton(GAMEPAD_BUTTON_LEFT_FACE_RIGHT, Action::MoveRight, p);
        bindGamepadButton(GAMEPAD_BUTTON_LEFT_FACE_UP, Action::Jump, p);
        bindGamepadButton(GAMEPAD_BUTTON_LEFT_FACE_DOWN, Action::Crouch, p);
        bindGamepadButton(GAMEPAD_BUTTON_RIGHT_FACE_DOWN, Action::Jump, p);     // A / Cross button to jump
        bindGamepadButton(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, Action::Run, p);     // B / Circle button to run
        bindGamepadButton(GAMEPAD_BUTTON_RIGHT_FACE_LEFT, Action::Shoot, p);    // X / Square button to shoot
        bindGamepadButton(GAMEPAD_BUTTON_RIGHT_TRIGGER_1, Action::Run, p);      // RB / R1 to run
        bindGamepadButton(GAMEPAD_BUTTON_MIDDLE_RIGHT, Action::Pause, p);       // Start button to pause
        bindGamepadButton(GAMEPAD_BUTTON_LEFT_FACE_UP, Action::MenuUp, p);
        bindGamepadButton(GAMEPAD_BUTTON_LEFT_FACE_DOWN, Action::MenuDown, p);
        bindGamepadButton(GAMEPAD_BUTTON_RIGHT_FACE_DOWN, Action::MenuConfirm, p);
    }
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

std::vector<std::pair<Action, int>>& InputManager::getPlayerGamepadBindings(int playerIndex) {
    if (playerIndex < 0) playerIndex = 0;
    if (playerIndex >= (int)gamepadBindingsPerPlayer.size()) {
        gamepadBindingsPerPlayer.resize(playerIndex + 1);
    }
    return gamepadBindingsPerPlayer[playerIndex];
}

const std::vector<std::pair<Action, int>>& InputManager::getPlayerGamepadBindings(int playerIndex) const {
    static const std::vector<std::pair<Action, int>> emptyBindings;
    if (playerIndex < 0 || playerIndex >= (int)gamepadBindingsPerPlayer.size()) {
        return emptyBindings;
    }
    return gamepadBindingsPerPlayer[playerIndex];
}

bool InputManager::bindGamepadButton(int button, Action action, int playerIndex) {
    auto& list = getPlayerGamepadBindings(playerIndex);
    for (auto& b : list) {
        if (b.first == action) {
            b.second = button;
            return true;
        }
    }
    list.push_back({action, button});
    return true;
}

bool InputManager::isGamepadAvailable(int playerIndex) const {
    return IsGamepadAvailable(playerIndex);
}

static bool isGameplayAction(Action action) {
    return action == Action::MoveLeft ||
           action == Action::MoveRight ||
           action == Action::Jump ||
           action == Action::Crouch ||
           action == Action::Run ||
           action == Action::Shoot;
}

bool InputManager::bindKey(int key, Action action, int playerIndex) {
    auto& list = getPlayerBindings(playerIndex);
    // Check if this key is already bound to any OTHER action in the same category for this player
    for (auto const& binding : list) {
        if (binding.second == key && binding.first != action) {
            if (isGameplayAction(action) && isGameplayAction(binding.first)) {
                return false; // Key exists for another gameplay action for this player
            }
            if (!isGameplayAction(action) && !isGameplayAction(binding.first)) {
                return false; // Key exists for another menu action for this player
            }
        }
    }

    // Check if this key is bound to a gameplay action for any other player (cross-player conflict)
    if (isGameplayAction(action)) {
        for (size_t p = 0; p < bindingsPerPlayer.size(); ++p) {
            if ((int)p != playerIndex) {
                for (auto const& binding : bindingsPerPlayer[p]) {
                    if (binding.second == key && isGameplayAction(binding.first)) {
                        return false; // Key is already used by another player for gameplay!
                    }
                }
            }
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
            if (isGameplayAction(action) && isGameplayAction(binding.first)) {
                return false;
            }
            if (!isGameplayAction(action) && !isGameplayAction(binding.first)) {
                return false;
            }
        }
        if (binding.second == key && binding.first == action) {
            return true; // Already bound to this action
        }
    }
    if (isGameplayAction(action)) {
        for (size_t p = 0; p < bindingsPerPlayer.size(); ++p) {
            if ((int)p != playerIndex) {
                for (auto const& binding : bindingsPerPlayer[p]) {
                    if (binding.second == key && isGameplayAction(binding.first)) {
                        return false; // Key is already used by another player for gameplay!
                    }
                }
            }
        }
    }
    list.push_back({action, key});
    return true;
}

void InputManager::update() {
    size_t maxPlayers = std::max(bindingsPerPlayer.size(), gamepadBindingsPerPlayer.size());
    if (actionStates.size() < maxPlayers) {
        actionStates.resize(maxPlayers);
    }
    if (prevActionStates.size() < maxPlayers) {
        prevActionStates.resize(maxPlayers);
    }

    for (size_t p = 0; p < maxPlayers; ++p) {
        prevActionStates[p] = actionStates[p];
        actionStates[p].clear();
        if (p < bindingsPerPlayer.size()) {
            for (auto const& binding : bindingsPerPlayer[p]) {
                if (IsKeyDown(binding.second)) {
                    actionStates[p][binding.first] = true;
                }
            }
        }
        if (IsGamepadAvailable((int)p) && p < gamepadBindingsPerPlayer.size()) {
            for (auto const& gb : gamepadBindingsPerPlayer[p]) {
                if (IsGamepadButtonDown((int)p, gb.second)) {
                    actionStates[p][gb.first] = true;
                }
            }
            float axisX = GetGamepadAxisMovement((int)p, GAMEPAD_AXIS_LEFT_X);
            float axisY = GetGamepadAxisMovement((int)p, GAMEPAD_AXIS_LEFT_Y);
            if (axisX < -0.3f) actionStates[p][Action::MoveLeft] = true;
            if (axisX > 0.3f) actionStates[p][Action::MoveRight] = true;
            if (axisY > 0.5f) {
                actionStates[p][Action::Crouch] = true;
                actionStates[p][Action::MenuDown] = true;
            }
            if (axisY < -0.5f) actionStates[p][Action::MenuUp] = true;
        }
    }
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
    if (IsGamepadAvailable(playerIndex)) {
        const auto& gList = getPlayerGamepadBindings(playerIndex);
        for (auto const& gb : gList) {
            if (gb.first == action && IsGamepadButtonDown(playerIndex, gb.second)) {
                return true;
            }
        }
        if (action == Action::MoveLeft && GetGamepadAxisMovement(playerIndex, GAMEPAD_AXIS_LEFT_X) < -0.3f) return true;
        if (action == Action::MoveRight && GetGamepadAxisMovement(playerIndex, GAMEPAD_AXIS_LEFT_X) > 0.3f) return true;
        if (action == Action::Crouch && GetGamepadAxisMovement(playerIndex, GAMEPAD_AXIS_LEFT_Y) > 0.5f) return true;
        if (action == Action::MenuDown && GetGamepadAxisMovement(playerIndex, GAMEPAD_AXIS_LEFT_Y) > 0.5f) return true;
        if (action == Action::MenuUp && GetGamepadAxisMovement(playerIndex, GAMEPAD_AXIS_LEFT_Y) < -0.5f) return true;
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
    if (IsGamepadAvailable(playerIndex)) {
        const auto& gList = getPlayerGamepadBindings(playerIndex);
        for (auto const& gb : gList) {
            if (gb.first == action && IsGamepadButtonPressed(playerIndex, gb.second)) {
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
    if (IsGamepadAvailable(playerIndex)) {
        const auto& gList = getPlayerGamepadBindings(playerIndex);
        for (auto const& gb : gList) {
            if (gb.first == action && IsGamepadButtonReleased(playerIndex, gb.second)) {
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

bool InputManager::getActionState(Action action, int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= (int)actionStates.size()) return false;
    auto it = actionStates[playerIndex].find(action);
    return it != actionStates[playerIndex].end() && it->second;
}

bool InputManager::getPrevActionState(Action action, int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= (int)prevActionStates.size()) return false;
    auto it = prevActionStates[playerIndex].find(action);
    return it != prevActionStates[playerIndex].end() && it->second;
}

std::string InputManager::actionToString(Action action) {
    switch (action) {
        case Action::MoveLeft: return "MoveLeft";
        case Action::MoveRight: return "MoveRight";
        case Action::Jump: return "Jump";
        case Action::Crouch: return "Crouch";
        case Action::Run: return "Run";
        case Action::Shoot: return "Shoot";
        case Action::Pause: return "Pause";
        case Action::MenuUp: return "MenuUp";
        case Action::MenuDown: return "MenuDown";
        case Action::MenuConfirm: return "MenuConfirm";
        default: return "Unknown";
    }
}

Action InputManager::stringToAction(const std::string& str) {
    if (str == "MoveLeft") return Action::MoveLeft;
    if (str == "MoveRight") return Action::MoveRight;
    if (str == "Jump") return Action::Jump;
    if (str == "Crouch") return Action::Crouch;
    if (str == "Run") return Action::Run;
    if (str == "Shoot") return Action::Shoot;
    if (str == "Pause") return Action::Pause;
    if (str == "MenuUp") return Action::MenuUp;
    if (str == "MenuDown") return Action::MenuDown;
    if (str == "MenuConfirm") return Action::MenuConfirm;
    return Action::Jump;
}

bool InputManager::isValidActionString(const std::string& str) {
    return (str == "MoveLeft" || str == "MoveRight" || str == "Jump" ||
            str == "Crouch" || str == "Run" || str == "Shoot" ||
            str == "Pause" || str == "MenuUp" || str == "MenuDown" ||
            str == "MenuConfirm");
}
