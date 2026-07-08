#ifndef LEVEL_SELECT_STATE_H
#define LEVEL_SELECT_STATE_H

#include "Core/GameState.hpp"
#include <vector>
#include <string>

class LevelSelectState : public GameState {
private:
    int selectedIndex;
    std::vector<std::string> levelFiles;
    float animTime;

public:
    LevelSelectState();
    ~LevelSelectState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // LEVEL_SELECT_STATE_H
