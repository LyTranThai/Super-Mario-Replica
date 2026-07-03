#include "GameplayState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "Core/GameEngine.h"
#include "Core/SoundManager.h"
#include "World/Level.h"
#include "Entities/Fireball.h"
#include "Entities/Item.h"
#include "Persistence/SaveManager.h"
#include <iostream>

struct FireballSpawnData {
    Vector2 position;
    bool facingRight;
};

struct ItemSpawnData {
    Vector2 position;
    ItemType type;
};

GameplayState::GameplayState() : GameplayState(0) {}

GameplayState::GameplayState(int startLevelIndex) : currentLevelIndex(startLevelIndex) {
    // Setup list of 3 levels
    levelFiles = { "assets/levels/level1.txt", "assets/levels/level2.txt", "assets/levels/level3.txt" };
}

GameplayState::~GameplayState() {
    EventManager::getInstance().unsubscribe(EventType::FireballShot, this);
    EventManager::getInstance().unsubscribe(EventType::ItemSpawned, this);
    EventManager::getInstance().unsubscribe(EventType::LevelCompleted, this);
    EventManager::getInstance().unsubscribe(EventType::PlayerDied, this);
}

void GameplayState::init() {
    if (currentLevelIndex < 0 || currentLevelIndex >= (int)levelFiles.size()) {
        currentLevelIndex = 0;
    }

    loadLevel(currentLevelIndex);

    // Subscribe to gameplay spawning events
    EventManager::getInstance().subscribe(EventType::FireballShot, this);
    EventManager::getInstance().subscribe(EventType::ItemSpawned, this);
    EventManager::getInstance().subscribe(EventType::LevelCompleted, this);
    EventManager::getInstance().subscribe(EventType::PlayerDied, this);
}

void GameplayState::loadLevel(int index) {
    level = std::make_unique<Level>(levelFiles[index]);
    
    // Play level theme sound
    SoundManager::getInstance().playMusic("level_theme");
}

void GameplayState::handleInput(const InputManager& input) {
    if (input.isActionJustPressed(Action::Pause)) {
        // Toggle pause overlay
        GameEngine::getInstance().getStateManager().pushState(new PauseState());
    } else {
        if (level && level->getPlayer()) {
            level->getPlayer()->handleInput(input);
        }
    }
}

void GameplayState::update(float dt) {
    if (pendingGameOver) {
        pendingGameOver = false;
        GameStateManager& gsm = GameEngine::getInstance().getStateManager();
        gsm.popState();
        gsm.pushState(new GameOverState(false, pendingScore));
        return;
    }
    if (pendingWin) {
        pendingWin = false;
        GameStateManager& gsm = GameEngine::getInstance().getStateManager();
        gsm.popState();
        gsm.pushState(new GameOverState(true, pendingScore));
        return;
    }
    if (pendingReset) {
        pendingReset = false;
        loadLevel(currentLevelIndex);
        if (level && level->getPlayer()) {
            level->getPlayer()->setLives(pendingLives);
            level->getPlayer()->addScore(pendingScore);
        }
        return;
    }

    if (level) {
        level->update(dt);
    }
}

void GameplayState::onBack() {
    GameEngine::getInstance().getStateManager().pushState(new PauseState());
}

void GameplayState::draw() {
    if (level) {
        level->draw();
    }
}

void GameplayState::onEvent(EventType type, void* data) {
    if (!level) return;

    switch (type) {
        case EventType::FireballShot: {
            FireballSpawnData* spawn = static_cast<FireballSpawnData*>(data);
            if (spawn) {
                level->spawnEntity(std::make_unique<Fireball>(spawn->position, spawn->facingRight));
            }
            break;
        }
        case EventType::ItemSpawned: {
            ItemSpawnData* spawn = static_cast<ItemSpawnData*>(data);
            if (spawn) {
                std::string tex = "mushroom";
                Color dbgCol = RED;
                if (spawn->type == ItemType::FireFlower) {
                    tex = "flower";
                    dbgCol = ORANGE;
                } else if (spawn->type == ItemType::Star) {
                    tex = "star";
                    dbgCol = GOLD;
                }
                level->spawnEntity(std::make_unique<Item>(spawn->position, spawn->type, tex, dbgCol));
            }
            break;
        }
        case EventType::LevelCompleted: {
            currentLevelIndex++;
            Account& acc = GameEngine::getInstance().getActiveAccount();
            
            // Record level highscore
            int completedIdx = currentLevelIndex - 1;
            int currentScore = level->getPlayer()->getScore();
            if (currentScore > acc.getLevelHighScore(completedIdx)) {
                acc.setLevelHighScore(completedIdx, currentScore);
            }

            // Record overall highscore
            if (currentScore > acc.getHighScore()) {
                acc.setHighScore(currentScore);
            }

            // Update progress settings if it unlocks a new level
            if (currentLevelIndex + 1 > acc.getCurrentLevel() && currentLevelIndex < (int)levelFiles.size()) {
                acc.setCurrentLevel(currentLevelIndex + 1);
            }
            SaveManager sm;
            sm.saveAccount(acc);
            
            // Always transition to level ending screen on completing any level
            pendingWin = true;
            pendingScore = currentScore;
            break;
        }
        case EventType::PlayerDied: {
            Player* p = level->getPlayer();
            int currentLives = p->getLives() - 1;
            if (currentLives <= 0) {
                // Trigger fail game over
                Account& acc = GameEngine::getInstance().getActiveAccount();
                int currentScore = p->getScore();
                if (currentScore > acc.getLevelHighScore(currentLevelIndex)) {
                    acc.setLevelHighScore(currentLevelIndex, currentScore);
                }
                if (currentScore > acc.getHighScore()) {
                    acc.setHighScore(currentScore);
                }
                SaveManager sm;
                sm.saveAccount(acc);
                pendingGameOver = true;
                pendingScore = currentScore;
            } else {
                // Respawn and reload level (deferred)
                pendingReset = true;
                pendingLives = currentLives;
                pendingScore = p->getScore();
            }
            break;
        }
        default:
            break;
    }
}
