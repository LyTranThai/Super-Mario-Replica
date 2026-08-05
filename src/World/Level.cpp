#include "Level.h"
#include "EntityFactory.h"
#include "Entities/RockHead.h"
#include "Core/EventSystem.h"
#include "Core/GameEngine.h"
#include "Core/AssetManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

Level::Level(const std::string& filePath) 
    : levelFilePath(filePath), levelWidth(800), levelHeight(600), isCompleted(false) {
    
    // Setup default viewport
    camera = GameCamera(Vector2{ 800.0f, 600.0f }, 0.0f, 2000.0f);
    loadFromFile(filePath);
}

#include <cstdlib>
#include <ctime>

void Level::loadFromFile(const std::string& filePath) {
    entities.clear();
    player.reset();
    sceneryTrees.clear();
    sceneryBushes.clear();
    sceneryBigHills.clear();
    scenerySmallHills.clear();
    sceneryClouds.clear();

    std::vector<std::string> lines;
    
    if (filePath == "RANDOM" || filePath == "assets/levels/random_level.txt") {
        int width = 100;
        int height = 15;
        lines = std::vector<std::string>(height, std::string(width, '.'));
        
        srand((unsigned int)time(NULL));

        // Right boundary wall & ground
        for (int r = 0; r < height; ++r) {
            lines[r][width - 1] = '#';
        }
        for (int c = 0; c < width; ++c) {
            lines[height - 1][c] = '#';
            lines[height - 2][c] = '#';
        }

        // Random pit gaps in ground
        for (int c = 18; c < width - 15; c += 16 + rand() % 8) {
            int pitWidth = 2 + rand() % 3;
            for (int p = 0; p < pitWidth && c + p < width - 10; ++p) {
                lines[height - 1][c + p] = '.';
                lines[height - 2][c + p] = '.';
            }
        }

        // Spawn & Exit Pipe
        lines[height - 3][3] = 'P';
        lines[height - 3][width - 3] = 'W';

        // Random platforms, items, and enemies
        for (int c = 10; c < width - 10; ++c) {
            if (rand() % 7 == 0) {
                int r = height - 6 - (rand() % 3);
                lines[r][c] = (rand() % 2 == 0) ? '?' : 'B';
                if (rand() % 4 == 0) lines[r][c] = 'M';
                else if (rand() % 5 == 0) lines[r][c] = 'F';
                else if (rand() % 9 == 0) lines[r][c] = 'S';
            }
            
            // Enemy spawns on solid ground
            if (rand() % 10 == 0 && lines[height - 1][c] == '#') {
                int enemyChoice = rand() % 4;
                if (enemyChoice == 0) lines[height - 3][c] = 'G';
                else if (enemyChoice == 1) lines[height - 3][c] = 'K';
                else if (enemyChoice == 2) lines[height - 3][c] = 'I';
                else if (enemyChoice == 3 && c % 18 == 0) lines[1][c] = 'T';
            }
        }
    } else {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Level file not found: " << filePath << ". Loading fallback level." << std::endl;
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
                // Spawn player with character type from active account
                std::string selChar = GameEngine::getInstance().getActiveAccount().getSelectedCharacter();
                CharacterType cType = (selChar == "Luigi") ? CharacterType::Luigi : CharacterType::Mario;
                player = std::make_unique<Player>(Vector2{ x, y }, cType);
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

    // Calculate scenery positions based on ground topology
    for (int col = 0; col < maxCols; ++col) {
        int topRow = -1;
        for (size_t row = 0; row < lines.size(); ++row) {
            if (col < lines[row].length() && lines[row][col] == '#') {
                topRow = (int)row;
                break;
            }
        }

        if (topRow != -1) {
            float x = col * TILE_SIZE;
            float y = topRow * TILE_SIZE;
            
            if (col % 22 == 15) {
                sceneryTrees.push_back(Vector2{ x, y - 64.0f });
            } else if (col % 16 == 5) {
                sceneryBushes.push_back(Vector2{ x, y - 32.0f });
            } else if (col % 28 == 0) {
                sceneryBigHills.push_back(Vector2{ x - 16.0f, y - 64.0f }); // slightly offset for visual variety
            } else if (col % 28 == 10) {
                scenerySmallHills.push_back(Vector2{ x - 16.0f, y - 48.0f });
            }
        }
        
        if (col % 12 == 0) {
            sceneryClouds.push_back(Vector2{ (float)col * TILE_SIZE, 40.0f + (col % 3) * 20.0f });
        }
    }

    // If no player was spawned, spawn at fallback default
    if (!player) {
        std::string selChar = GameEngine::getInstance().getActiveAccount().getSelectedCharacter();
        CharacterType cType = (selChar == "Luigi") ? CharacterType::Luigi : CharacterType::Mario;
        player = std::make_unique<Player>(Vector2{ 100.0f, 100.0f }, cType);
    }
}

#include "Entities/ExitBlock.h"

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
            
            // Check Exit Pipe / Goal Door collision
            ExitBlock* exitBlock = dynamic_cast<ExitBlock*>(entity.get());
            if (exitBlock && player->isActive()) {
                if (CheckCollisionRecs(player->getBoundingBox(), exitBlock->getBoundingBox())) {
                    isCompleted = true;
                    EventManager::getInstance().broadcast(EventType::LevelCompleted);
                }
            }

            entity->update(dt);
        }
    }

    // 3. Evaluate and resolve AABB collisions
    if (player->isActive()) {
        collisionChecker.updatePhysics(entities, *player, dt);
    }

    // 4. Remove deactivated entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(), 
            [](const std::unique_ptr<Entity>& e) { return !e->isActive(); }), 
        entities.end()
    );

    // 5. Update Camera target focus locked relative to player
    if (player->isActive()) {
        // Enforce right boundary wall clamping
        float maxPlayerX = levelWidth - TILE_SIZE - player->getHitboxSize().x;
        if (player->getPosition().x > maxPlayerX) {
            Vector2 p = player->getPosition();
            p.x = maxPlayerX;
            player->setPosition(p);
        }
        if (player->getPosition().x < 0.0f) {
            Vector2 p = player->getPosition();
            p.x = 0.0f;
            player->setPosition(p);
        }

        camera.update(player->getPosition());

        // Check if player reached the rightmost end of the map (completion checkpoint)
        if (player->getPosition().x >= levelWidth - 64.0f) {
            isCompleted = true;
            EventManager::getInstance().broadcast(EventType::LevelCompleted);
        }
    }
}

void Level::drawScenery() {
    Texture2D worldTex = AssetManager::getInstance().getTexture("world");
    if (worldTex.id == 0) return;

    // Source rects in World.png
    Rectangle srcSmallHill = { 0.0f, 160.0f, 80.0f, 48.0f };
    Rectangle srcBigHill   = { 176.0f, 144.0f, 80.0f, 64.0f };
    Rectangle srcBush      = { 256.0f, 176.0f, 48.0f, 32.0f };
    Rectangle srcCloud     = { 128.0f, 48.0f, 48.0f, 32.0f };
    Rectangle srcTree      = { 448.0f, 144.0f, 32.0f, 64.0f };

    for (const auto& pos : sceneryBigHills) {
        Vector2 off = camera.applyOffset(pos);
        DrawTexturePro(worldTex, srcBigHill, Rectangle{ off.x, off.y, 160.0f, 128.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    for (const auto& pos : scenerySmallHills) {
        Vector2 off = camera.applyOffset(pos);
        DrawTexturePro(worldTex, srcSmallHill, Rectangle{ off.x, off.y, 120.0f, 96.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    for (const auto& pos : sceneryBushes) {
        Vector2 off = camera.applyOffset(pos);
        DrawTexturePro(worldTex, srcBush, Rectangle{ off.x, off.y, 96.0f, 64.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    for (const auto& pos : sceneryTrees) {
        Vector2 off = camera.applyOffset(pos);
        DrawTexturePro(worldTex, srcTree, Rectangle{ off.x, off.y, 64.0f, 128.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    for (const auto& pos : sceneryClouds) {
        Vector2 off = camera.applyOffset(pos);
        DrawTexturePro(worldTex, srcCloud, Rectangle{ off.x, off.y, 96.0f, 64.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
}

void Level::draw() {
    ClearBackground(Color{ 92, 148, 252, 255 });

    // 0. Draw background scenery (mountains, trees, bushes, clouds)
    drawScenery();

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
