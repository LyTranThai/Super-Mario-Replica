#ifndef GAME_STATE_H
#define GAME_STATE_H

class InputManager;

class GameState {
public:
    virtual ~GameState() = default;
    virtual void init() = 0;
    virtual void handleInput(const InputManager& input) = 0;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual void onBack() {}
};

#endif // GAME_STATE_H
