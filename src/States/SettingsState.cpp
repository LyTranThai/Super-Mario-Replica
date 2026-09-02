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
    : selectedIndex(0), selectedPlayerIndex(0), isRebinding(false), errorMessage("") {}

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

            bool success = GameEngine::getInstance().getInputManager().bindKey(key, act, selectedPlayerIndex);
            if (success) {
                activeAccount.setKeySetting(activeActionToRebind, key, selectedPlayerIndex);
                GameEngine::getInstance().setActiveAccount(activeAccount);
                saveManager.saveAccount(activeAccount); // Persist updated key configurations
                isRebinding = false;
                errorMessage = "";
            } else {
                errorMessage = "Key already bound to another action or player!";
            }
        }
    } else {
        if (input.isActionJustPressed(Action::MenuUp)) {
            selectedIndex = (selectedIndex - 1 + settingsOptions.size()) % settingsOptions.size();
        }
        if (input.isActionJustPressed(Action::MenuDown)) {
            selectedIndex = (selectedIndex + 1) % settingsOptions.size();
        }
        if (IsKeyPressed(KEY_TAB) || input.isActionJustPressed(Action::MoveLeft) || input.isActionJustPressed(Action::MoveRight)) {
            selectedPlayerIndex = 1 - selectedPlayerIndex;
            errorMessage = "";
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
    
    DrawText("SUPER MARIO OOP", 150, 40, 46, MAROON);
    DrawText("KEYBOARD SETTINGS", 280, 95, 20, DARKGRAY);

    if (isRebinding) {
        std::string pName = (selectedPlayerIndex == 0) ? "PLAYER 1 (MARIO)" : "PLAYER 2 (LUIGI)";
        DrawText(("REBINDING FOR " + pName).c_str(), 200, 180, 24, MAROON);
        std::string msg = "Press any key to bind to: " + activeActionToRebind;
        DrawText(msg.c_str(), 160, 260, 22, BLACK);
        DrawText("Press [ESC] to Cancel", 280, 330, 20, DARKGRAY);
        if (!errorMessage.empty()) {
            DrawText(errorMessage.c_str(), 130, 400, 20, RED);
        }
    } else {
        std::string tabHeader = (selectedPlayerIndex == 0) ? "< [TAB] PLAYER 1 (MARIO) >" : "< [TAB] PLAYER 2 (LUIGI) >";
        Color headerColor = (selectedPlayerIndex == 0) ? RED : GREEN;
        DrawText(tabHeader.c_str(), 240, 140, 24, headerColor);

        for (size_t i = 0; i < settingsOptions.size(); ++i) {
            Color color = (i == (size_t)selectedIndex) ? headerColor : BLACK;
            std::string label = settingsOptions[i];
            Action act = InputManager::stringToAction(label);
            int keyCode = GameEngine::getInstance().getInputManager().getBoundKey(act, selectedPlayerIndex);
            if (keyCode == 0) {
                keyCode = activeAccount.getKeySetting(label, selectedPlayerIndex);
            }
            label = label + " : [" + getKeyNameForSettings(keyCode) + "]";
            
            std::string prefix = (i == (size_t)selectedIndex) ? "> " : "  ";
            DrawText((prefix + label).c_str(), 240, 185 + i * 36, 22, color);
        }
        
        DrawText("Press [TAB] or [A/D] to Switch Player | [Esc] to Return", 150, 560, 18, DARKGRAY);
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
