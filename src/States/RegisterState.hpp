#ifndef REGISTER_STATE_H
#define REGISTER_STATE_H

#include "Core/GameState.hpp"
#include "Persistence/SaveManager.hpp"
#include <string>

class RegisterState : public GameState {
private:
    std::string inputUsername;
    std::string inputPassword;
    bool isPasswordHidden;
    bool isTypingUsername;
    std::string errorMessage;

    SaveManager saveManager;

    void drawTextBox(const std::string& label, const std::string& text, int x, int y, bool active, bool isPassword);

public:
    RegisterState();
    ~RegisterState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // REGISTER_STATE_H
