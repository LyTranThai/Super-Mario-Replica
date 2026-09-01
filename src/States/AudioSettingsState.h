#ifndef AUDIO_SETTINGS_STATE_H
#define AUDIO_SETTINGS_STATE_H

#include "Core/GameState.h"
#include <vector>
#include <string>

class AudioSettingsState : public GameState {
private:
    int selectedIndex;
    std::vector<std::string> options;
    float animTime;
    int currentTestSoundIndex;
    std::vector<std::string> testSoundIds;

    void adjustVolume(int direction);
    void triggerAction();

public:
    AudioSettingsState();
    ~AudioSettingsState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;
};

#endif // AUDIO_SETTINGS_STATE_H
