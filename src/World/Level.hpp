#ifndef LEVEL_H
#define LEVEL_H

#include "Entities/Entity.hpp"
#include "Entities/Player.hpp"
#include "Physics/Camera.hpp"
#include "Physics/CollisionManager.hpp"
#include <vector>
#include <memory>
#include <string>

class Level {
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::unique_ptr<Player> player;
    GameCamera camera;
    CollisionManager collisionManager;
    
    std::string levelFilePath;
    int levelWidth;
    int levelHeight;
    bool isCompleted;

    static constexpr float TILE_SIZE = 32.0f;

public:
    Level(const std::string& filePath);
    ~Level() = default;

    void loadFromFile(const std::string& filePath);
    void update(float dt);
    void draw();

    void spawnEntity(std::unique_ptr<Entity> newEntity);
    Player* getPlayer() const { return player.get(); }
    GameCamera& getCamera() { return camera; }

    bool isLevelCompleted() const { return isCompleted; }
    void setCompleted(bool state) { isCompleted = state; }
};

#endif // LEVEL_H
