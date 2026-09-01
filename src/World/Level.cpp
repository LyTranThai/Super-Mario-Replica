#include "Level.h"
#include <cmath>
#include "Entities/ExitBlock.h"
#include "Entities/TeleportPipe.h"
#include "Core/InputManager.h"

#include "EntityFactory.h"
#include "Entities/Player.h"
#include "Entities/Mario.h"
#include "Entities/Luigi.h"
#include "Entities/Block.h"
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

void Level::generateChunk(float startX, float endX) {
    int startCol = (int)(startX / TILE_SIZE);
    int endCol = (int)(endX / TILE_SIZE);
    int width = endCol - startCol;
    if (width <= 0) return;
    int height = 15;
    
    std::vector<std::string> lines(height, std::string(width, '.'));
    
    for (int c = 0; c < width; ++c) {
        lines[height - 1][c] = '#';
    }
    
    int c = 0;
    while (c < width) {
        int globalCol = startCol + c;
        if (globalCol > 18 && (rand() % 15 == 0)) { 
            int pitWidth = 2 + rand() % 3;
            for (int p = 0; p < pitWidth && c + p < width; ++p) {
                lines[height - 1][c + p] = '.';
                lines[height - 2][c + p] = '.';
            }
            c += pitWidth + (10 + rand() % 10);
            continue;
        }
        
        if (globalCol > 10) {
            // New algorithm: check if we are far enough from the last segment
            // "at least 20 greater than x, 20 smaller than y"
            // Since columns are 32px, 20 columns would be huge. 
            // We'll enforce a distance of at least 20 pixels in X (which is 1 column)
            // But if they meant 20 columns, globalCol >= lastSegmentEndX + 20 does that.
            // Let's assume they meant 20 pixels (~1 tile) for both, or 20 columns?
            // We'll use 20 columns as X distance to space them out nicely, 
            // and at least 20 pixels (~1 tile) higher in Y.
            // To prevent going out of bounds at the top, we'll reset to bottom if needed.
            if (globalCol >= lastSegmentEndX + 20) {
                // Calculate new Y. "20 smaller than y" => y_pixel <= last_y_pixel - 20.
                int newY = lastSegmentY - 1; // at least 1 tile (32px) higher, which is > 20px
                if (newY < 2 || lastSegmentY == 0) {
                    newY = height - 5 - (rand() % 4); // Reset height if it reaches the ceiling
                }

                int segmentLength = 1 + (rand() % 6); // Random length (1 to 6)
                int startType = 1 + (rand() % 2); // 1 = Brick, 2 = Mystery

                for (int i = 0; i < segmentLength && c + i < width; ++i) {
                    bool isBrick = ((startType == 1 && i % 2 == 0) || (startType == 2 && i % 2 != 0));
                    
                    if (isBrick) {
                        lines[newY][c + i] = 'B';
                    } else {
                        // 50% Coin ('?'), 20% Mushroom ('M'), 20% Fire Flower ('F'), 5% Star ('S'), 5% Heart ('L')
                        int roll = rand() % 100;
                        if (roll < 50) {
                            lines[newY][c + i] = '?';
                        } else if (roll < 70) {
                            lines[newY][c + i] = 'M';
                        } else if (roll < 90) {
                            lines[newY][c + i] = 'F';
                        } else if (roll < 95) {
                            lines[newY][c + i] = 'S';
                        } else {
                            lines[newY][c + i] = 'L';
                        }
                    }
                }
                
                lastSegmentEndX = globalCol + segmentLength - 1;
                lastSegmentY = newY;
                
                // Skip the loop counter forward by the segment length we just drew
                // (subtract 1 because the while loop does c++ later)
                c += segmentLength - 1; 
            }
            
            if (rand() % 4 == 0 && lines[height - 1][c] == '#') {
                int enemyChoice = rand() % 4;
                if (enemyChoice == 0) lines[height - 3][c] = 'G';
                else if (enemyChoice == 1) lines[height - 3][c] = 'K';
                else if (enemyChoice == 2) lines[height - 3][c] = 'I';
                else if (enemyChoice == 3 && rand() % 3 == 0) lines[1][c] = 'T';
            }
        }
        c++;
    }
    
    for (size_t row = 0; row < height; ++row) {
        for (size_t c = 0; c < width; ++c) {
            char type = lines[row][c];
            int globalCol = startCol + c;
            float x = globalCol * TILE_SIZE;
            float y = row * TILE_SIZE;
            
            if (type != '.' && type != ' ' && type != '\n') {
                auto ent = EntityFactory::createEntity(type, x, y);
                if (ent) {
                    if (type == '#') {
                        bool isTop = (row == 0 || lines[row-1][c] != '#');
                        bool isFloating = true;
                        for (size_t r = row; r < height; ++r) {
                            if (lines[r][c] == '#') {
                                if (r == height - 1) isFloating = false;
                            } else {
                                break;
                            }
                        }
                        if (Block* b = dynamic_cast<Block*>(ent.get())) {
                            b->isTopGround = isTop;
                            if (isFloating) b->blockType = Block::Type::FlyingBrick;
                        }
                    }
                    entities.push_back(std::move(ent));
                }
            }
        }
    }
    
    for (int c = 0; c < width; ++c) {
        int globalCol = startCol + c;
        int groundTopRow = -1;
        for (int row = height - 1; row >= 0; --row) {
            if (lines[row][c] == '#') groundTopRow = row;
            else break;
        }
        
        if (groundTopRow != -1) {
            auto checkFlatGround = [&](int localStart, int widthTiles) {
                for (int lc = localStart; lc < localStart + widthTiles && lc < width; ++lc) {
                    int cRow = -1;
                    for (int r = height - 1; r >= 0; --r) {
                        if (lines[r][lc] == '#') cRow = r;
                        else break;
                    }
                    if (cRow != groundTopRow) return false;
                }
                return true;
            };
            
            float x = globalCol * TILE_SIZE;
            float y = groundTopRow * TILE_SIZE;
            
            if (rand() % 25 == 0 && checkFlatGround(c, 3)) sceneryBush1.push_back(Vector2{ x, y - 16.0f });
            else if (rand() % 20 == 0 && checkFlatGround(c, 2)) sceneryBush2.push_back(Vector2{ x, y - 19.0f });
            else if (rand() % 35 == 0 && c > 0 && checkFlatGround(c - 1, 14)) sceneryBigHills.push_back(Vector2{ x - 32.0f, y - 184.0f });
            else if (rand() % 30 == 0 && checkFlatGround(c, 8)) scenerySmallHills.push_back(Vector2{ x - 16.0f, y - 188.0f });
        }
        
        if (rand() % 15 == 0) sceneryClouds.push_back(Vector2{ (float)globalCol * TILE_SIZE, 60.0f + (rand() % 3) * 20.0f });
        else if (rand() % 20 == 0) sceneryNbClouds1.push_back(Vector2{ (float)globalCol * TILE_SIZE, 10.0f + (rand() % 2) * 15.0f });
        else if (rand() % 25 == 0) sceneryNbClouds2.push_back(Vector2{ (float)globalCol * TILE_SIZE, 20.0f + (rand() % 3) * 10.0f });
    }
}

std::unique_ptr<Player> Level::createPlayer(Vector2 pos) {
    std::string selChar = GameEngine::getInstance().getActiveAccount().getSelectedCharacter();
    if (selChar == "Luigi") {
        return std::make_unique<Luigi>(pos);
    } else {
        return std::make_unique<Mario>(pos);
    }
}

void Level::loadFromFile(const std::string& filePath) {
    entities.clear();
    players.clear();
    respawnTimers.clear();
    sceneryBigHills.clear();
    scenerySmallHills.clear();
    sceneryBush1.clear();
    sceneryBush2.clear();
    sceneryClouds.clear();
    sceneryNbClouds1.clear();
    sceneryNbClouds2.clear();

    isInfinite = false;
    currentGenerationX = 0.0f;
    lastSegmentEndX = 0;
    lastSegmentY = 0;
    camera.setLeftLocked(false);

    if (filePath == "RANDOM" || filePath == "assets/levels/random_level.txt") {
        isInfinite = true;
        srand((unsigned int)time(NULL));
        
        levelWidth = 999999;
        levelHeight = 15 * TILE_SIZE;
        camera.setBoundaries(0.0f, 999999.0f);
        camera.setLeftLocked(true);
        
        // Two-Player Co-op: Mario is Player 1, Luigi is Player 2 (5 lives each)
        auto mario = std::make_unique<Mario>(Vector2{ 100.0f, 100.0f });
        mario->setPlayerIndex(0);
        mario->setLives(5);

        auto luigi = std::make_unique<Luigi>(Vector2{ 150.0f, 100.0f });
        luigi->setPlayerIndex(1);
        luigi->setLives(5);

        players.push_back(std::move(mario));
        players.push_back(std::move(luigi));
        respawnTimers.assign(players.size(), 0.0f);
        
        generateChunk(0.0f, 1600.0f);
        currentGenerationX = 1600.0f;
        return;
    }

    std::vector<std::string> lines;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Level file not found: " << filePath << ". Loading fallback level." << std::endl;
        lines = {
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
            "X                                                          X",
            "X                                                          X",
            "X                                                          X",
            "X                                                          X",
            "X        ?  B  M  F                                        X",
            "X                                                          X",
            "X                                 T                        X",
            "X                                                          X",
            "X    P       G       C           XXX         N             X",
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
                if (players.empty()) {
                    auto p = createPlayer(Vector2{ x, y });
                    p->setLives(5);
                    players.push_back(std::move(p));
                    respawnTimers.push_back(0.0f);
                }
            } else if (type != ' ' && type != '\n') {
                auto ent = EntityFactory::createEntity(type, x, y);
                if (ent) {
                    if (type == '#') {
                        bool isTop = (row == 0 || col >= lines[row-1].length() || lines[row-1][col] != '#');
                        bool isFloating = true;
                        for (size_t r = row; r < lines.size(); ++r) {
                            if (col < lines[r].length() && lines[r][col] == '#') {
                                if (r == lines.size() - 1) isFloating = false;
                            } else {
                                break;
                            }
                        }
                        if (Block* b = dynamic_cast<Block*>(ent.get())) {
                            b->isTopGround = isTop;
                            if (isFloating) b->blockType = Block::Type::FlyingBrick;
                        }
                    }
                    entities.push_back(std::move(ent));
                }
            }
        }
    }

    levelWidth = maxCols * TILE_SIZE;
    levelHeight = lines.size() * TILE_SIZE;
    camera.setBoundaries(0.0f, levelWidth);

    for (int col = 0; col < maxCols; ++col) {
        int groundTopRow = -1;
        for (int row = (int)lines.size() - 1; row >= 0; --row) {
            if (col < lines[row].length() && lines[row][col] == '#') {
                groundTopRow = row;
            } else {
                break;
            }
        }

        if (groundTopRow != -1) {
            auto checkFlatGround = [&](int startCol, int widthTiles) {
                for (int c = startCol; c < startCol + widthTiles && c < maxCols; ++c) {
                    int cRow = -1;
                    for (int r = (int)lines.size() - 1; r >= 0; --r) {
                        if (c < lines[r].length() && lines[r][c] == '#') {
                            cRow = r;
                        } else {
                            break;
                        }
                    }
                    if (cRow != groundTopRow) return false;
                }
                return true;
            };

            float x = col * TILE_SIZE;
            float y = groundTopRow * TILE_SIZE;
            
            if (col % 22 == 15) {
                if (checkFlatGround(col, 3)) sceneryBush1.push_back(Vector2{ x, y - 16.0f });
            } else if (col % 16 == 5) {
                if (checkFlatGround(col, 2)) sceneryBush2.push_back(Vector2{ x, y - 19.0f });
            } else if (col % 28 == 0) {
                if (checkFlatGround(col - 1, 14)) sceneryBigHills.push_back(Vector2{ x - 32.0f, y - 184.0f });
            } else if (col % 28 == 10) {
                if (checkFlatGround(col, 8)) scenerySmallHills.push_back(Vector2{ x - 16.0f, y - 188.0f });
            }
        }
        
        if (col % 12 == 0) {
            sceneryClouds.push_back(Vector2{ (float)col * TILE_SIZE, 60.0f + (col % 3) * 20.0f });
        } else if (col % 18 == 6) {
            sceneryNbClouds1.push_back(Vector2{ (float)col * TILE_SIZE, 10.0f + (col % 2) * 15.0f });
        } else if (col % 24 == 14) {
            sceneryNbClouds2.push_back(Vector2{ (float)col * TILE_SIZE, 20.0f + (col % 3) * 10.0f });
        }
    }

    if (players.empty()) {
        auto p = createPlayer(Vector2{ 100.0f, 100.0f });
        p->setLives(5);
        players.push_back(std::move(p));
        respawnTimers.push_back(0.0f);
    }
}

void Level::update(float dt) {
    if (isCompleted) return;

    // 1. Update all players coordinates
    for (auto& p : players) {
        if (p && p->isActive()) {
            p->update(dt);
        }
    }

    // 2. Active triggering (e.g. check RockHead thwomp slam bounds)
    for (auto& entity : entities) {
        if (entity->isActive()) {
            RockHead* thwomp = dynamic_cast<RockHead*>(entity.get());
            if (thwomp) {
                for (auto& p : players) {
                    if (p && p->isActive()) thwomp->checkTrigger(p->getPosition());
                }
            }
            
            // Check Exit Pipe / Goal Door collision
            ExitBlock* exitBlock = dynamic_cast<ExitBlock*>(entity.get());
            if (exitBlock) {
                for (auto& p : players) {
                    if (!p || !p->isActive() || p->isPiping()) continue;
                    bool crouchPressed = GameEngine::getInstance().getInputManager().isActionPressed(Action::Crouch, p->getPlayerIndex());
                    Rectangle pBox = p->getBoundingBox();
                    pBox.height += 2.0f;
                    Rectangle tBox = exitBlock->getBoundingBox();
                    if (CheckCollisionRecs(pBox, tBox) && crouchPressed && std::abs((pBox.y + pBox.height - 2.0f) - tBox.y) < 5.0f) {
                        p->startPiping(Vector2{0,0}, true);
                    }
                }
            }
            
            TeleportPipe* telePipe = dynamic_cast<TeleportPipe*>(entity.get());
            if (telePipe) {
                for (auto& p : players) {
                    if (!p || !p->isActive() || p->isPiping()) continue;
                    bool crouchPressed = GameEngine::getInstance().getInputManager().isActionPressed(Action::Crouch, p->getPlayerIndex());
                    Rectangle pBox = p->getBoundingBox();
                    pBox.height += 2.0f;
                    Rectangle tBox = telePipe->getBoundingBox();
                    if (CheckCollisionRecs(pBox, tBox) && crouchPressed && std::abs((pBox.y + pBox.height - 2.0f) - tBox.y) < 5.0f) {
                        std::vector<TeleportPipe*> allPipes;
                        for (auto& e : entities) {
                            if (auto pipe = dynamic_cast<TeleportPipe*>(e.get())) {
                                if (pipe != telePipe) allPipes.push_back(pipe);
                            }
                        }
                        Vector2 target = telePipe->getPosition();
                        if (!allPipes.empty()) {
                            int r = rand() % allPipes.size();
                            target = allPipes[r]->getPosition();
                        }
                        p->startPiping(target, false);
                    }
                }
            }

            entity->update(dt);
        }
    }

    // 3. Evaluate and resolve AABB collisions
    for (auto& p : players) {
        if (p && p->isActive()) {
            collisionChecker.updatePhysics(entities, *p, dt);
        }
    }

    // Pairwise player-to-player collision (boost bounce on head)
    for (size_t i = 0; i < players.size(); ++i) {
        for (size_t j = i + 1; j < players.size(); ++j) {
            if (players[i] && players[j] && players[i]->isActive() && players[j]->isActive()) {
                CollisionChecker::checkPlayerPlayerCollision(*players[i], *players[j]);
            }
        }
    }

    // 4. Remove deactivated entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(), 
            [](const std::unique_ptr<Entity>& e) { return !e->isActive(); }), 
        entities.end()
    );

    // 5. Update Camera target & viewport boundaries
    if (isInfinite) {
        // Chunk generation based on furthest active player
        float maxLeadX = 0.0f;
        for (auto& p : players) {
            if (p && p->isActive()) {
                maxLeadX = std::max(maxLeadX, p->getPosition().x);
            }
        }
        
        if (maxLeadX + 1600.0f > currentGenerationX) {
            generateChunk(currentGenerationX, currentGenerationX + 800.0f);
            currentGenerationX += 800.0f;
        }

        // Cull offscreen entities and scenery
        float gcThreshold = camera.getPosition().x - 1000.0f;
        entities.erase(std::remove_if(entities.begin(), entities.end(), [&](const std::unique_ptr<Entity>& e) {
            return e->getPosition().x < gcThreshold;
        }), entities.end());

        auto cullScenery = [&](std::vector<Vector2>& vec) {
            vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const Vector2& v) { return v.x < gcThreshold; }), vec.end());
        };
        cullScenery(sceneryBigHills);
        cullScenery(scenerySmallHills);
        cullScenery(sceneryBush1);
        cullScenery(sceneryBush2);
        cullScenery(sceneryClouds);
        cullScenery(sceneryNbClouds1);
        cullScenery(sceneryNbClouds2);

        // CAMERA MOVEMENT RULE:
        // In order to move the camera right, all active players have to simultaneously move right.
        // If one player stands, and another moves right, the camera doesn't move right.
        int activeCount = 0;
        int movingRightCount = 0;
        float trailingX = 999999.0f;

        for (auto& p : players) {
            if (p && p->isActive()) {
                activeCount++;
                if (p->getVelocity().x > 10.0f) {
                    movingRightCount++;
                }
                trailingX = std::min(trailingX, p->getPosition().x);
            }
        }

        if (activeCount > 0 && movingRightCount == activeCount) {
            // All active players are moving right simultaneously: advance camera tracking trailing player
            camera.update(Vector2{ trailingX + 200.0f, 0.0f }, dt);
        }

        // Viewport clamping: keep all players within visible screen frame
        float screenLeft = camera.getPosition().x;
        float screenRight = camera.getPosition().x + (float)GetScreenWidth();

        for (auto& p : players) {
            if (!p || !p->isActive()) continue;
            Vector2 pos = p->getPosition();
            Vector2 vel = p->getVelocity();
            if (pos.x < screenLeft) {
                pos.x = screenLeft;
                vel.x = 0.0f;
            }
            float rightEdge = screenRight - p->getHitboxSize().x;
            if (pos.x > rightEdge) {
                pos.x = rightEdge;
                vel.x = 0.0f;
            }
            p->setPosition(pos);
            p->setVelocity(vel);
        }

        // Pit fall check & co-op respawn
        for (size_t i = 0; i < players.size(); ++i) {
            auto& p = players[i];
            if (p && p->isActive() && p->getPosition().y > levelHeight + 100.0f) {
                p->setActive(false);
                p->setLives(p->getLives() - 1);
                if (i < respawnTimers.size()) {
                    respawnTimers[i] = 3.0f;
                }
            }
        }

        // Respawn inactive players who still have lives near an active teammate
        for (size_t i = 0; i < players.size(); ++i) {
            auto& p = players[i];
            if (p && !p->isActive() && p->getLives() > 0) {
                if (i < respawnTimers.size()) {
                    respawnTimers[i] -= dt;
                    if (respawnTimers[i] <= 0.0f) {
                        Player* anchor = nullptr;
                        for (auto& mate : players) {
                            if (mate && mate->isActive()) {
                                anchor = mate.get();
                                break;
                            }
                        }
                        if (anchor) {
                            p->setActive(true);
                            Vector2 spawnPos = { anchor->getPosition().x, std::max(50.0f, anchor->getPosition().y - 80.0f) };
                            p->setPosition(spawnPos);
                            p->setVelocity(Vector2{ 0.0f, 0.0f });
                            p->addInvincibility(2.0f);
                        } else {
                            // Both players are inactive! Respawn at top-left of current screen
                            p->setActive(true);
                            float spawnX = camera.getPosition().x + 60.0f + i * 50.0f;
                            
                            // Check if spawnX is above a pit; if so, shift forward to safe ground
                            bool groundBelow = false;
                            for (auto const& ent : entities) {
                                if (ent && ent->isSolid() && std::abs(ent->getPosition().x - spawnX) < 24.0f && ent->getPosition().y > 300.0f) {
                                    groundBelow = true;
                                    break;
                                }
                            }
                            if (!groundBelow) {
                                spawnX += 130.0f;
                            }

                            Vector2 spawnPos = { spawnX, 60.0f };
                            p->setPosition(spawnPos);
                            p->setVelocity(Vector2{ 0.0f, 0.0f });
                            p->addInvincibility(3.0f);
                        }
                    }
                }
            }
        }

        // Game Over when all players have zero lives and are inactive
        bool anyCanPlay = false;
        for (auto& p : players) {
            if (p && (p->isActive() || p->getLives() > 0)) {
                anyCanPlay = true;
                break;
            }
        }
        if (!anyCanPlay) {
            EventManager::getInstance().broadcast(EventType::PlayerDied);
        }
    } else {
        // Standard single player level logic
        Player* p = getPlayer(0);
        if (p && p->isActive()) {
            float maxPlayerX = levelWidth - TILE_SIZE - p->getHitboxSize().x;
            if (p->getPosition().x > maxPlayerX) {
                Vector2 pos = p->getPosition();
                pos.x = maxPlayerX;
                p->setPosition(pos);
            }
            if (p->getPosition().x < 0.0f) {
                Vector2 pos = p->getPosition();
                pos.x = 0.0f;
                p->setPosition(pos);
            }

            if (p->getPosition().y > levelHeight + 100.0f) {
                EventManager::getInstance().broadcast(EventType::PlayerDied);
                p->setActive(false);
            } else {
                camera.update(p->getPosition(), dt);
            }
        }
    }

    // 6. Add any entities spawned during the update step to prevent iterator invalidation
    if (!pendingEntities.empty()) {
        for (auto& newEnt : pendingEntities) {
            entities.push_back(std::move(newEnt));
        }
        pendingEntities.clear();
    }
}
void Level::drawScenery() {
    // Fill the lower layer below the ground
    float groundBottomY = camera.applyOffset(Vector2{0.0f, (float)levelHeight}).y;
    DrawRectangle(0, (int)groundBottomY, GetScreenWidth(), GetScreenHeight(), Color{ 192, 248, 248, 255 });

    Texture2D bgTex = AssetManager::getInstance().getTexture("background");
    if (bgTex.id != 0) {
        Rectangle srcBg = { 18.0f, 18.0f, 3071.0f, 238.0f }; // (18,18)->(3089,256)
        Vector2 off = camera.applyOffset(Vector2{0.0f, 0.0f});
        
        float destHeight = 238.0f * 2.0f; // 476.0f
        float destY = GetScreenHeight() - destHeight;
        
        if (isInfinite) {
            float bgWidth = srcBg.width;
            float startX = -fmod(camera.getPosition().x * 0.2f, bgWidth);
            for(int i = 0; i < 3; i++) {
                DrawTexturePro(bgTex, srcBg, Rectangle{ startX + i * bgWidth, destY, bgWidth, destHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        } else {
            DrawTexturePro(bgTex, srcBg, Rectangle{ off.x, destY, (float)levelWidth, destHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
    }

    Texture2D worldTex = AssetManager::getInstance().getTexture("world");
    Texture2D hillTex = AssetManager::getInstance().getTexture("hill");
    Texture2D mountTex = AssetManager::getInstance().getTexture("mountain");
    Texture2D nbCloudTex = AssetManager::getInstance().getTexture("nobackgroundcloud");

    Rectangle srcBigHill   = { 12.0f, 139.0f, 212.0f, 92.0f }; // Mountain: (12,139) -> (224,231)
    Rectangle srcSmallHill = { 48.0f, 160.0f, 120.0f, 94.0f }; // Hill: (48,160) -> (168,254)
    Rectangle srcBush1     = { 182.0f, 193.0f, 67.0f, 16.0f };
    Rectangle srcBush2     = { 1431.0f, 190.0f, 52.0f, 19.0f };
    Rectangle srcCloud     = { 128.0f, 48.0f, 48.0f, 32.0f };
    Rectangle srcNbCloud1  = { 388.0f, 15.0f, 108.0f, 77.0f };
    Rectangle srcNbCloud2  = { 830.0f, 16.0f, 83.0f, 66.0f };

    for (const auto& pos : sceneryBigHills) {
        Vector2 off = camera.applyOffset(pos);
        if (mountTex.id != 0) DrawTexturePro(mountTex, srcBigHill, Rectangle{ off.x, off.y, 424.0f, 184.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    for (const auto& pos : scenerySmallHills) {
        Vector2 off = camera.applyOffset(pos);
        if (hillTex.id != 0) DrawTexturePro(hillTex, srcSmallHill, Rectangle{ off.x, off.y, 240.0f, 188.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    if (nbCloudTex.id != 0) {
        for (const auto& pos : sceneryNbClouds1) {
            Vector2 off = camera.applyOffset(pos);
            DrawTexturePro(nbCloudTex, srcNbCloud1, Rectangle{ off.x, off.y, 216.0f, 154.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        for (const auto& pos : sceneryNbClouds2) {
            Vector2 off = camera.applyOffset(pos);
            DrawTexturePro(nbCloudTex, srcNbCloud2, Rectangle{ off.x, off.y, 166.0f, 132.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
    }
    if (worldTex.id != 0) {
        for (const auto& pos : sceneryBush1) {
            Vector2 off = camera.applyOffset(pos);
            DrawTexturePro(worldTex, srcBush1, Rectangle{ off.x, off.y, 67.0f, 16.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        for (const auto& pos : sceneryBush2) {
            Vector2 off = camera.applyOffset(pos);
            DrawTexturePro(worldTex, srcBush2, Rectangle{ off.x, off.y, 52.0f, 19.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        for (const auto& pos : sceneryClouds) {
            Vector2 off = camera.applyOffset(pos);
            DrawTexturePro(worldTex, srcCloud, Rectangle{ off.x, off.y, 96.0f, 64.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
    }
}

void Level::draw() {
    ClearBackground(Color{ 80, 136, 160, 255 });

    // 0. Draw background scenery (mountains, trees, bushes, clouds)
    drawScenery();

    // 1. Draw Players FIRST if piping so they appear behind pipes
    for (auto& p : players) {
        if (p && p->isActive() && p->isPiping()) {
            Vector2 origPos = p->getPosition();
            Vector2 offsetPos = camera.applyOffset(origPos);
            p->setPosition(offsetPos);
            p->draw();
            p->setPosition(origPos);
        }
    }

    // 2. Draw entities relative to Camera offsets
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

    // 3. Draw Players AFTER entities if NOT piping
    for (auto& p : players) {
        if (p && p->isActive() && !p->isPiping()) {
            Vector2 origPos = p->getPosition();
            Vector2 offsetPos = camera.applyOffset(origPos);
            p->setPosition(offsetPos);
            p->draw();
            p->setPosition(origPos);
        }
    }

    // 4. Draw static UI elements overlay (HUD) at fixed positions
    DrawRectangle(10, 10, 780, 40, Fade(BLACK, 0.45f));
    
    if (players.size() > 1) {
        Player* p1 = getPlayer(0);
        Player* p2 = getPlayer(1);
        std::string mStr = "MARIO  LIVES: " + std::to_string(p1 ? p1->getLives() : 0) + 
                           "  SCORE: " + std::to_string(p1 ? p1->getScore() : 0);
        std::string lStr = "LUIGI  LIVES: " + std::to_string(p2 ? p2->getLives() : 0) + 
                           "  SCORE: " + std::to_string(p2 ? p2->getScore() : 0);
        DrawText(mStr.c_str(), 30, 20, 18, RED);
        DrawText(lStr.c_str(), 430, 20, 18, GREEN);
    } else {
        Player* p = getPlayer(0);
        std::string scoreStr = "SCORE: " + std::to_string(p ? p->getScore() : 0);
        std::string coinsStr = "COINS: " + std::to_string(p ? p->getCoins() : 0);
        std::string livesStr = "LIVES: " + std::to_string(p ? p->getLives() : 0);
        
        DrawText(scoreStr.c_str(), 30, 20, 20, WHITE);
        DrawText(coinsStr.c_str(), 250, 20, 20, YELLOW);
        DrawText(livesStr.c_str(), 450, 20, 20, RED);
        DrawText("LEVEL ACTIVE", 600, 20, 20, LIGHTGRAY);
    }
}

void Level::spawnEntity(std::unique_ptr<Entity> newEntity) {
    if (newEntity) {
        pendingEntities.push_back(std::move(newEntity));
    }
}
