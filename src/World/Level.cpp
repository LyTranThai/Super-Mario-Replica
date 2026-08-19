#include "Level.h"
#include <cmath>
#include "Entities/ExitBlock.h"
#include "Entities/TeleportPipe.h"
#include "Core/InputManager.h"

#include "EntityFactory.h"
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
            if (rand() % 3 == 0) {
                int r = height - 6 - (rand() % 3);
                lines[r][c] = (rand() % 2 == 0) ? '?' : 'B';
                if (rand() % 4 == 0) lines[r][c] = 'M';
                else if (rand() % 5 == 0) lines[r][c] = 'F';
                else if (rand() % 9 == 0) lines[r][c] = 'S';
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

void Level::loadFromFile(const std::string& filePath) {
    entities.clear();
    player.reset();
    sceneryBigHills.clear();
    scenerySmallHills.clear();
    sceneryBush1.clear();
    sceneryBush2.clear();
    sceneryClouds.clear();
    sceneryNbClouds1.clear();
    sceneryNbClouds2.clear();

    isInfinite = false;
    currentGenerationX = 0.0f;
    camera.setLeftLocked(false);

    if (filePath == "RANDOM" || filePath == "assets/levels/random_level.txt") {
        isInfinite = true;
        srand((unsigned int)time(NULL));
        
        levelWidth = 999999;
        levelHeight = 15 * TILE_SIZE;
        camera.setBoundaries(0.0f, 999999.0f);
        camera.setLeftLocked(true);
        
        std::string selChar = GameEngine::getInstance().getActiveAccount().getSelectedCharacter();
        CharacterType cType = (selChar == "Luigi") ? CharacterType::Luigi : CharacterType::Mario;
        player = std::make_unique<Player>(Vector2{ 100.0f, 100.0f }, cType);
        
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
            "X    P       G       K           XXX         I             X",
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
                std::string selChar = GameEngine::getInstance().getActiveAccount().getSelectedCharacter();
                CharacterType cType = (selChar == "Luigi") ? CharacterType::Luigi : CharacterType::Mario;
                player = std::make_unique<Player>(Vector2{ x, y }, cType);
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

    if (!player) {
        std::string selChar = GameEngine::getInstance().getActiveAccount().getSelectedCharacter();
        CharacterType cType = (selChar == "Luigi") ? CharacterType::Luigi : CharacterType::Mario;
        player = std::make_unique<Player>(Vector2{ 100.0f, 100.0f }, cType);
    }

    // Add 2-3 random Teleport Pipes
    std::vector<Block*> topGrounds;
    for (auto& entity : entities) {
        if (Block* b = dynamic_cast<Block*>(entity.get())) {
            if (b->isTopGround && b->blockType == Block::Type::Ground) {
                // Ensure it's not too close to the edges
                if (b->getPosition().x > 200.0f && b->getPosition().x < levelWidth - 400.0f) {
                    topGrounds.push_back(b);
                }
            }
        }
    }
    
    if (topGrounds.size() > 10) {
        // Simple shuffle
        for (size_t i = 0; i < topGrounds.size(); ++i) {
            size_t j = i + rand() % (topGrounds.size() - i);
            std::swap(topGrounds[i], topGrounds[j]);
        }
        int numPipes = 2 + rand() % 2;
        for (int i = 0; i < numPipes && i < (int)topGrounds.size(); ++i) {
            Vector2 pos = topGrounds[i]->getPosition();
            pos.y -= 64.0f; // Place on top of ground block
            entities.push_back(std::make_unique<TeleportPipe>(pos));
        }
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
            
            bool crouchPressed = GameEngine::getInstance().getInputManager().isActionPressed(Action::Crouch);

            // Check Exit Pipe / Goal Door collision
            ExitBlock* exitBlock = dynamic_cast<ExitBlock*>(entity.get());
            if (exitBlock && player->isActive() && !player->isPiping()) {
                Rectangle pBox = player->getBoundingBox();
                pBox.height += 2.0f; // Inflate to detect standing
                Rectangle tBox = exitBlock->getBoundingBox();
                if (CheckCollisionRecs(pBox, tBox) && crouchPressed && std::abs((pBox.y + pBox.height - 2.0f) - tBox.y) < 5.0f) {
                    player->startPiping(Vector2{0,0}, true);
                }
            }
            
            TeleportPipe* telePipe = dynamic_cast<TeleportPipe*>(entity.get());
            if (telePipe && player->isActive() && !player->isPiping()) {
                Rectangle pBox = player->getBoundingBox();
                pBox.height += 2.0f; // Inflate to detect standing
                Rectangle tBox = telePipe->getBoundingBox();
                if (CheckCollisionRecs(pBox, tBox) && crouchPressed && std::abs((pBox.y + pBox.height - 2.0f) - tBox.y) < 5.0f) {
                    std::vector<TeleportPipe*> allPipes;
                    for (auto& e : entities) {
                        if (auto p = dynamic_cast<TeleportPipe*>(e.get())) {
                            if (p != telePipe) allPipes.push_back(p);
                        }
                    }
                    Vector2 target = telePipe->getPosition(); // Fallback
                    if (!allPipes.empty()) {
                        int r = rand() % allPipes.size();
                        target = allPipes[r]->getPosition();
                    }
                    player->startPiping(target, false);
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
        if (isInfinite) {
            if (player->getPosition().x + 1600.0f > currentGenerationX) {
                generateChunk(currentGenerationX, currentGenerationX + 800.0f);
                currentGenerationX += 800.0f;
            }
            
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
            
            if (player->getPosition().x < camera.getPosition().x) {
                Vector2 p = player->getPosition();
                p.x = camera.getPosition().x;
                player->setPosition(p);
                player->setVelocity(Vector2{0, player->getVelocity().y});
            }
        } else {
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
            if (player->getPosition().x >= levelWidth - 64.0f) {
                isCompleted = true;
                EventManager::getInstance().broadcast(EventType::LevelCompleted);
            }
        }

        camera.update(player->getPosition(), dt);
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

    // 1. Draw Player FIRST if piping so he appears behind pipes
    bool playerPiping = player->isActive() && player->isPiping();
    if (playerPiping) {
        Vector2 origPos = player->getPosition();
        Vector2 offsetPos = camera.applyOffset(origPos);
        player->setPosition(offsetPos);
        player->draw();
        player->setPosition(origPos);
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

    // 3. Draw Player AFTER entities if NOT piping
    if (player->isActive() && !playerPiping) {
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
