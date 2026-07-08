#include "GameStateManager.hpp"
#include "GameState.hpp"
#include "raylib.h"

GameStateManager::~GameStateManager() {
    processPendingActions();
    for (GameState* state : states) {
        delete state;
    }
    states.clear();
}

void GameStateManager::pushState(GameState* state) {
    pendingActions.push_back({StateActionType::PUSH, state});
}

void GameStateManager::popState() {
    pendingActions.push_back({StateActionType::POP, nullptr});
}

void GameStateManager::changeState(GameState* state) {
    pendingActions.push_back({StateActionType::CHANGE, state});
}

void GameStateManager::clear() {
    pendingActions.push_back({StateActionType::CLEAR, nullptr});
}

void GameStateManager::processPendingActions() {
    for (auto const& action : pendingActions) {
        switch (action.type) {
            case StateActionType::PUSH:
                states.push_back(action.state);
                action.state->init();
                break;
            case StateActionType::POP:
                if (!states.empty()) {
                    GameState* top = states.back();
                    states.pop_back();
                    delete top;
                }
                break;
            case StateActionType::CHANGE:
                while (!states.empty()) {
                    GameState* top = states.back();
                    states.pop_back();
                    delete top;
                }
                states.push_back(action.state);
                action.state->init();
                break;
            case StateActionType::CLEAR:
                while (!states.empty()) {
                    GameState* top = states.back();
                    states.pop_back();
                    delete top;
                }
                break;
        }
    }
    pendingActions.clear();
}

void GameStateManager::handleInput(const InputManager& input) {
    if (!states.empty()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            states.back()->onBack();
        } else {
            states.back()->handleInput(input);
        }
    }
}

void GameStateManager::update(float dt) {
    if (!states.empty()) {
        states.back()->update(dt);
    }
}

void GameStateManager::draw() {
    if (!states.empty()) {
        states.back()->draw();
    }
}
