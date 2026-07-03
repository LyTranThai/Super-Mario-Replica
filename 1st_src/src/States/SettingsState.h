#ifndef SETTINGS_STATE_H
#define SETTINGS_STATE_H

#include "Core/GameState.h"
#include "Persistence/SaveManager.h"
#include "Persistence/Account.h"
#include <string>
#include <vector>

class SettingsState : public GameState {
private:
    int selectedIndex;
    std::vector<std::string> settingsOptions;

    bool isRebinding;
    std::string activeActionToRebind;
    Account activeAccount;
    SaveManager saveManager;
    std::string errorMessage;

public:
    SettingsState();
    ~SettingsState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // SETTINGS_STATE_H
