#ifndef PAUSE_STATE_H
#define PAUSE_STATE_H

#include "Core/GameState.h"
#include <vector>
#include <string>

class PauseState : public GameState {
private:
    int selectedIndex;
    std::vector<std::string> options;

public:
    PauseState();
    ~PauseState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // PAUSE_STATE_H
