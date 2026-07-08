#include "Level.hpp"
#include "EntityFactory.hpp"
#include "Entities/RockHead.hpp"
#include "Core/EventSystem.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

Level::Level(const std::string& filePath) 
    : levelFilePath(filePath), levelWidth(800), levelHeight(600), isCompleted(false) {
    
    // Setup default viewport
    camera = GameCamera(Vector2{ 800.0f, 600.0f }, 0.0f, 2000.0f);
    loadFromFile(filePath);
}

void Level::loadFromFile(const std::string& filePath) {
    entities.clear();
    player.reset();

    std::ifstream file(filePath);
    std::vector<std::string> lines;
    
    if (!file.is_open()) {
        std::cerr << "Level file not found: " << filePath << ". Loading fallback level." << std::endl;
        // Fallback grid
        lines = {
            "############################################################",
            "#                                                          #",
            "#                                                          #",
            "#                                                          #",
            "#                                                          #",
            "#        ?  B  M  F                                        #",
            "#                                                          #",
            "#                                 T                        #",
            "#                                                          #",
            "#    P       G       K           ###         I             #",
            "############################################################"
        };
    } else {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back(); // Clean carriage return in Windows text files
            }
            lines.push_back(line);
        }
        file.close();
    }

    if (lines.empty()) return;

    int maxCols = 0;
    for (size_t row = 0; row < lines.size(); ++row) {
        std::string const& line = lines[row];
        maxCols = std::max(maxCols, (int)line.length());
        for (size_t col = 0; col < line.length(); ++col) {
            char type = line[col];
            float x = col * TILE_SIZE;
            float y = row * TILE_SIZE;

            if (type == 'P') {
                // Spawn player
                player = std::make_unique<Player>(Vector2{ x, y });
            } else if (type != ' ' && type != '\n') {
                // Spawn environment/enemy blocks via factory
                auto ent = EntityFactory::createEntity(type, x, y);
                if (ent) {
                    entities.push_back(std::move(ent));
                }
            }
        }
    }

    levelWidth = maxCols * TILE_SIZE;
    levelHeight = lines.size() * TILE_SIZE;
    camera.setBoundaries(0.0f, levelWidth);

    // If no player was spawned, spawn at fallback default
    if (!player) {
        player = std::make_unique<Player>(Vector2{ 100.0f, 100.0f });
    }
}

void Level::update(float dt) {
    if (isCompleted) return;

    // 1. Update player coordinates
    if (player->isActive()) {
        player->update(dt);
    }

    // 2. Active triggering (e.g. check RockHead thwomp slam bounds)
    for (auto& entity : entities) {
        if (entity->isActive()) {
            RockHead* thwomp = dynamic_cast<RockHead*>(entity.get());
            if (thwomp && player->isActive()) {
                thwomp->checkTrigger(player->getPosition());
            }
            entity->update(dt);
        }
    }

    // 3. Evaluate and resolve AABB collisions
    if (player->isActive()) {
        collisionManager.updatePhysicsAndCollisions(entities, *player, dt);
    }

    // 4. Remove deactivated entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(), 
            [](const std::unique_ptr<Entity>& e) { return !e->isActive(); }), 
        entities.end()
    );

    // 5. Update Camera target focus locked relative to player
    if (player->isActive()) {
        camera.update(player->getPosition());

        // Check if player reached the rightmost end of the map (completion checkpoint)
        if (player->getPosition().x >= levelWidth - 64.0f) {
            isCompleted = true;
            EventManager::getInstance().broadcast(EventType::LevelCompleted);
        }
    }
}

void Level::draw() {
    // 1. Draw entities relative to Camera offsets
    for (auto& entity : entities) {
        if (entity->isActive()) {
            Vector2 origPos = entity->getPosition();
            Vector2 offsetPos = camera.applyOffset(origPos);
            
            // Render relative coordinates
            entity->setPosition(offsetPos);
            entity->draw();
            entity->setPosition(origPos); // Reset back to original world space
        }
    }

    // 2. Draw Player relative to Camera offsets
    if (player->isActive()) {
        Vector2 origPos = player->getPosition();
        Vector2 offsetPos = camera.applyOffset(origPos);

        player->setPosition(offsetPos);
        player->draw();
        player->setPosition(origPos);
    }

    // 3. Draw static UI elements overlay (HUD) at fixed positions
    DrawRectangle(10, 10, 780, 40, Fade(BLACK, 0.4f));
    
    std::string scoreStr = "SCORE: " + std::to_string(player->getScore());
    std::string coinsStr = "COINS: " + std::to_string(player->getCoins());
    std::string livesStr = "LIVES: " + std::to_string(player->getLives());
    
    DrawText(scoreStr.c_str(), 30, 20, 20, WHITE);
    DrawText(coinsStr.c_str(), 250, 20, 20, YELLOW);
    DrawText(livesStr.c_str(), 450, 20, 20, RED);
    DrawText("LEVEL ACTIVE", 600, 20, 20, LIGHTGRAY);
}

void Level::spawnEntity(std::unique_ptr<Entity> newEntity) {
    if (newEntity) {
        entities.push_back(std::move(newEntity));
    }
}
