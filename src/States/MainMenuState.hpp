#ifndef MAIN_MENU_STATE_H
#define MAIN_MENU_STATE_H

#include "Core/GameState.hpp"
#include "Persistence/Account.hpp"
#include <string>
#include <vector>

class MainMenuState : public GameState {
private:
    int selectedIndex;
    std::vector<std::string> mainOptions;
    Account activeAccount;

public:
    MainMenuState();
    ~MainMenuState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // MAIN_MENU_STATE_H
