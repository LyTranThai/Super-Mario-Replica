#include "SettingsState.h"
#include "raylib.h"
#include "Core/GameEngine.h"

// Simple helper to convert Raylib keycodes to strings
static std::string getKeyNameForSettings(int key) {
    if (key >= KEY_A && key <= KEY_Z) {
        return std::string(1, (char)('A' + (key - KEY_A)));
    }
    if (key >= KEY_ZERO && key <= KEY_NINE) {
        return std::string(1, (char)('0' + (key - KEY_ZERO)));
    }
    if (key >= KEY_KP_0 && key <= KEY_KP_9) {
        return "NUM_" + std::to_string(key - KEY_KP_0);
    }
    switch (key) {
        case KEY_SPACE: return "SPACE";
        case KEY_ESCAPE: return "ESCAPE";
        case KEY_ENTER: return "ENTER";
        case KEY_TAB: return "TAB";
        case KEY_BACKSPACE: return "BACKSPACE";
        case KEY_LEFT_SHIFT: return "L_SHIFT";
        case KEY_RIGHT_SHIFT: return "R_SHIFT";
        case KEY_LEFT_CONTROL: return "L_CTRL";
        case KEY_RIGHT_CONTROL: return "R_CTRL";
        case KEY_LEFT_ALT: return "L_ALT";
        case KEY_RIGHT_ALT: return "R_ALT";
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_SLASH: return "/";
        case KEY_BACKSLASH: return "\\";
        case KEY_SEMICOLON: return ";";
        case KEY_COMMA: return ",";
        case KEY_PERIOD: return ".";
        case KEY_MINUS: return "-";
        case KEY_EQUAL: return "=";
        case KEY_GRAVE: return "`";
        case KEY_LEFT_BRACKET: return "[";
        case KEY_RIGHT_BRACKET: return "]";
        case KEY_KP_DIVIDE: return "NUM_/";
        case KEY_KP_MULTIPLY: return "NUM_*";
        case KEY_KP_SUBTRACT: return "NUM_-";
        case KEY_KP_ADD: return "NUM_+";
        case KEY_KP_DECIMAL: return "NUM_.";
        case KEY_KP_ENTER: return "NUM_ENTER";
        case KEY_KP_EQUAL: return "NUM_=";
        default: return "KEY_" + std::to_string(key);
    }
}

SettingsState::SettingsState()
    : selectedIndex(0), isRebinding(false), errorMessage("") {}

void SettingsState::init() {
    settingsOptions = { "MoveLeft", "MoveRight", "Jump", "Crouch", "Run", "Shoot", "Pause", "MenuUp", "MenuDown" };
    activeAccount = GameEngine::getInstance().getActiveAccount();
}

void SettingsState::handleInput(const InputManager& input) {
    if (isRebinding) {
        int key = GetKeyPressed();
        if (key > 0) {
            if (key == KEY_ESCAPE) {
                isRebinding = false;
                errorMessage = "";
                return;
            }

            if (!InputManager::isValidActionString(activeActionToRebind)) return;
            Action act = InputManager::stringToAction(activeActionToRebind);

            bool success = GameEngine::getInstance().getInputManager().bindKey(key, act);
            if (success) {
                activeAccount.setKeySetting(activeActionToRebind, key);
                GameEngine::getInstance().setActiveAccount(activeAccount);
                saveManager.saveAccount(activeAccount); // Persist updated key configurations
                isRebinding = false;
                errorMessage = "";
            } else {
                errorMessage = "Key already bound to another action!";
            }
        }
    } else {
        if (input.isActionJustPressed(Action::MenuUp)) {
            selectedIndex = (selectedIndex - 1 + settingsOptions.size()) % settingsOptions.size();
        }
        if (input.isActionJustPressed(Action::MenuDown)) {
            selectedIndex = (selectedIndex + 1) % settingsOptions.size();
        }
        if (input.isActionJustPressed(Action::MenuConfirm)) {
            activeActionToRebind = settingsOptions[selectedIndex];
            isRebinding = true;
            errorMessage = "";
        }
    }
}

void SettingsState::update(float dt) {
    (void)dt;
}

void SettingsState::draw() {
    ClearBackground(RAYWHITE);
    
    DrawText("SUPER MARIO OOP", 150, 50, 50, MAROON);
    DrawText("C++ & Raylib Project", 250, 110, 20, DARKGRAY);

    if (isRebinding) {
        DrawText("REBINDING KEY...", 250, 200, 30, MAROON);
        std::string msg = "Press any key to bind to: " + activeActionToRebind;
        DrawText(msg.c_str(), 150, 280, 25, BLACK);
        DrawText("Press [ESC] to Cancel", 280, 340, 20, DARKGRAY);
        if (!errorMessage.empty()) {
            DrawText(errorMessage.c_str(), 180, 400, 20, RED);
        }
    } else {
        DrawText("CUSTOM KEY BINDINGS", 250, 150, 25, DARKGRAY);
        for (size_t i = 0; i < settingsOptions.size(); ++i) {
            Color color = (i == (size_t)selectedIndex) ? RED : BLACK;
            std::string label = settingsOptions[i];
            int keyCode = activeAccount.getKeySetting(label);
            label = label + " : [" + getKeyNameForSettings(keyCode) + "]";
            
            std::string prefix = (i == (size_t)selectedIndex) ? "> " : "  ";
            DrawText((prefix + label).c_str(), 200, 200 + i * 40, 25, color);
        }
        
        DrawText("Press [Esc] to Return to Menu", 250, 560, 20, DARKGRAY);
    }
}

void SettingsState::onBack() {
    if (isRebinding) {
        isRebinding = false;
        errorMessage = "";
    } else {
        GameEngine::getInstance().getStateManager().popState();
    }
}
