#ifndef LEVEL_H
#define LEVEL_H

#include "Entities/Entity.h"
#include "Entities/Player.h"
#include "Physics/Camera.h"
#include "Physics/CollisionChecker.h"
#include <vector>
#include <memory>
#include <string>

class Level {
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> pendingEntities;
    std::vector<std::unique_ptr<Player>> players;
    std::vector<float> respawnTimers;
    GameCamera camera;
    CollisionChecker collisionChecker;
    
    std::string levelFilePath;
    int levelWidth;
    int levelHeight;
    bool isCompleted;
    bool isInfinite;
    float currentGenerationX;
    int lastSegmentEndX;
    int lastSegmentY;

    void generateChunk(float startX, float endX);
    std::unique_ptr<Player> createPlayer(Vector2 pos);

    std::vector<Vector2> sceneryBigHills;
    std::vector<Vector2> scenerySmallHills;
    std::vector<Vector2> sceneryBush1;
    std::vector<Vector2> sceneryBush2;
    std::vector<Vector2> sceneryClouds;
    std::vector<Vector2> sceneryNbClouds1;
    std::vector<Vector2> sceneryNbClouds2;

    static constexpr float TILE_SIZE = 32.0f;
    void drawScenery();

public:
    Level(const std::string& filePath);
    ~Level() = default;

    void loadFromFile(const std::string& filePath);
    void update(float dt);
    void draw();

    void spawnEntity(std::unique_ptr<Entity> newEntity);
    Player* getPlayer(size_t index = 0) const { return (index < players.size()) ? players[index].get() : nullptr; }
    Player* getPlayer2() const { return getPlayer(1); }
    const std::vector<std::unique_ptr<Player>>& getPlayers() const { return players; }
    GameCamera& getCamera() { return camera; }

    bool isLevelCompleted() const { return isCompleted; }
    void setCompleted(bool state) { isCompleted = state; }
};

#endif // LEVEL_H
