#include "LoginState.h"
#include "raylib.h"
#include "Core/GameEngine.h"
#include "States/LevelSelectState.h"
#include "Persistence/Account.h"

LoginState::LoginState()
    : inputUsername(""), inputPassword(""), isPasswordHidden(true), 
      isTypingUsername(true), errorMessage("") {}

void LoginState::init() {
    // Initialization if necessary
}

void LoginState::handleInput(const InputManager& input) {
    (void)input;

    // Toggle fields with Tab
    if (IsKeyPressed(KEY_TAB)) {
        isTypingUsername = !isTypingUsername;
        return;
    }

    // Process typing inputs
    std::string& targetString = isTypingUsername ? inputUsername : inputPassword;
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (targetString.length() < 15)) {
            targetString += (char)key;
        }
        key = GetCharPressed();
    }

    // Backspace logic
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!targetString.empty()) {
            targetString.pop_back();
        }
    }

    // Enter confirms
    if (IsKeyPressed(KEY_ENTER)) {
        if (saveManager.verifyCredentials(inputUsername, inputPassword)) {
            Account loaded;
            saveManager.loadAccount(inputUsername, loaded);
            GameEngine::getInstance().setActiveAccount(loaded);
            
            // Rebind keys in engine key bindings
            for (auto const& [actName, keyCode] : loaded.getKeySettings()) {
                Action act;
                if (actName == "MoveLeft") act = Action::MoveLeft;
                else if (actName == "MoveRight") act = Action::MoveRight;
                else if (actName == "Jump") act = Action::Jump;
                else if (actName == "Crouch") act = Action::Crouch;
                else if (actName == "Run") act = Action::Run;
                else if (actName == "Shoot") act = Action::Shoot;
                else if (actName == "Pause") act = Action::Pause;
                else if (actName == "MenuUp") act = Action::MenuUp;
                else if (actName == "MenuDown") act = Action::MenuDown;
                else continue;
                GameEngine::getInstance().getInputManager().bindKey(keyCode, act);
            }

            // After login, push LevelSelectState
            GameEngine::getInstance().getStateManager().pushState(new LevelSelectState());
        } else {
            errorMessage = "Invalid username or password!";
        }
    }
}

void LoginState::update(float dt) {
    (void)dt;
}

void LoginState::drawTextBox(const std::string& label, const std::string& text, int x, int y, bool active, bool isPassword) {
    DrawText(label.c_str(), x, y, 20, DARKGRAY);
    DrawRectangle(x, y + 25, 250, 35, active ? LIGHTGRAY : GRAY);
    DrawRectangleLines(x, y + 25, 250, 35, active ? RED : DARKGRAY);
    
    std::string displayText = text;
    if (isPassword) {
        displayText = std::string(text.length(), '*');
    }
    DrawText(displayText.c_str(), x + 10, y + 32, 20, BLACK);
}

void LoginState::draw() {
    ClearBackground(RAYWHITE);
    
    DrawText("SUPER MARIO OOP", 150, 50, 50, MAROON);
    DrawText("C++ & Raylib Project", 250, 110, 20, DARKGRAY);

    DrawText("MEMBER PROFILE LOGIN", 250, 160, 25, MAROON);
    
    drawTextBox("Username:", inputUsername, 275, 220, isTypingUsername, false);
    drawTextBox("Password:", inputPassword, 275, 300, !isTypingUsername, isPasswordHidden);

    DrawText("[Tab] Switch Fields  |  [Enter] Confirm  |  [Esc] Back", 160, 400, 20, DARKGRAY);
    if (!errorMessage.empty()) {
        DrawText(errorMessage.c_str(), 200, 450, 20, RED);
    }
}

void LoginState::onBack() {
    GameEngine::getInstance().getStateManager().popState();
}
