#include "LevelEditorState.h"
#include "Core/GameEngine.h"
#include "Core/SoundManager.h"
#include "States/GameplayState.h"
#include <fstream>
#include <iostream>
#include <cmath>

LevelEditorState::LevelEditorState()
    : gridWidth(100), gridHeight(15), cameraX(0.0f), selectedTileIndex(1), statusTimer(0.0f) {}

void LevelEditorState::init() {
    availableTiles = {
        { 'P', "Player Spawn", RED },
        { '#', "Solid Block", DARKGRAY },
        { 'B', "Brick Block", BROWN },
        { '?', "Question Coin", GOLD },
        { 'M', "Mushroom Block", RED },
        { 'F', "FireFlower Block", ORANGE },
        { 'G', "Goomba Enemy", PURPLE },
        { 'K', "Koopa Enemy", GREEN },
        { 'T', "Thwomp Crusher", GRAY },
        { 'I', "Piranha Plant", MAROON },
        { 'W', "Exit Warp Pipe", GREEN },
        { '.', "Eraser (Air)", LIGHTGRAY }
    };

    // Load existing custom level or initialize default blank template
    std::ifstream file("assets/levels/custom_level.txt");
    if (file.is_open()) {
        grid.clear();
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            grid.push_back(line);
        }
        file.close();
        if (!grid.empty()) {
            gridHeight = grid.size();
            gridWidth = grid[0].length();
        }
    }

    if (grid.empty()) {
        grid.clear();
        for (int r = 0; r < gridHeight; ++r) {
            std::string line = "";
            for (int c = 0; c < gridWidth; ++c) {
                if (c == gridWidth - 1) {
                    line += "#";
                } else if (r == gridHeight - 1) {
                    line += "#";
                } else if (r == gridHeight - 3 && c == gridWidth - 3) {
                    line += "W";
                } else if (r == gridHeight - 4 && c == 3) {
                    line += "P";
                } else {
                    line += ".";
                }
            }
            grid.push_back(line);
        }
    }

    statusMessage = "Use [WASD/Arrows] to scroll | Left-Click place | Right-Click erase | [S] Save | [T] Test Play";
    statusTimer = 4.0f;
}

void LevelEditorState::handleInput(const InputManager& input) {
    (void)input;

    // Scroll camera
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) cameraX += 400.0f * GetFrameTime();
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) cameraX -= 400.0f * GetFrameTime();

    float maxCamX = (gridWidth * 32.0f) - GetScreenWidth();
    if (maxCamX < 0) maxCamX = 0;
    if (cameraX < 0) cameraX = 0;
    if (cameraX > maxCamX) cameraX = maxCamX;

    // Quick select tile via number keys
    for (int k = 0; k <= 9; ++k) {
        if (IsKeyPressed(KEY_ZERO + k)) {
            if (k == 0) selectedTileIndex = 0; // P
            else if (k <= 9) selectedTileIndex = k;
        }
    }
    if (IsKeyPressed(KEY_E)) selectedTileIndex = (int)availableTiles.size() - 1; // Eraser

    // Mouse grid placement
    Vector2 mousePos = GetMousePosition();
    if (mousePos.y > 70 && mousePos.y < GetScreenHeight() - 60) {
        int col = (int)((mousePos.x + cameraX) / 32.0f);
        int row = (int)((mousePos.y - 70.0f) / 32.0f);

        if (row >= 0 && row < gridHeight && col >= 0 && col < gridWidth) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                char placeType = availableTiles[selectedTileIndex].type;
                // If placing Player spawn 'P', clear any existing 'P'
                if (placeType == 'P') {
                    for (int r = 0; r < gridHeight; ++r) {
                        for (int c = 0; c < gridWidth; ++c) {
                            if (grid[r][c] == 'P') grid[r][c] = '.';
                        }
                    }
                }
                grid[row][col] = placeType;
            } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                grid[row][col] = '.';
            }
        }
    }

    // Action Hotkeys
    if (IsKeyPressed(KEY_S)) {
        saveCustomLevel();
    }
    if (IsKeyPressed(KEY_T)) {
        saveCustomLevel();
        playCustomLevel();
    }
}

void LevelEditorState::update(float dt) {
    if (statusTimer > 0.0f) {
        statusTimer -= dt;
    }
}

void LevelEditorState::saveCustomLevel() {
    std::ofstream file("assets/levels/custom_level.txt");
    if (file.is_open()) {
        for (const auto& line : grid) {
            file << line << "\n";
        }
        file.close();
        statusMessage = "Custom Level Saved to assets/levels/custom_level.txt!";
        statusTimer = 3.0f;
        SoundManager::getInstance().playSound("coin");
    } else {
        statusMessage = "Error: Could not save custom level file!";
        statusTimer = 3.0f;
    }
}

void LevelEditorState::playCustomLevel() {
    GameEngine::getInstance().getStateManager().pushState(new GameplayState(3)); // Custom level slot
}

void LevelEditorState::onBack() {
    GameEngine::getInstance().getStateManager().popState();
}

void LevelEditorState::draw() {
    ClearBackground(Color{ 20, 20, 30, 255 });

    // Top Header UI Bar
    DrawRectangle(0, 0, GetScreenWidth(), 70, Color{ 35, 35, 45, 255 });
    DrawText("LEVEL EDITOR", 20, 15, 26, GOLD);

    // Render palette tiles
    for (size_t i = 0; i < availableTiles.size(); ++i) {
        int bx = 200 + i * 50;
        int by = 15;
        bool isSel = (i == (size_t)selectedTileIndex);

        Color boxBg = isSel ? YELLOW : Color{ 50, 50, 60, 255 };
        DrawRectangle(bx, by, 40, 40, boxBg);
        DrawRectangleLines(bx, by, 40, 40, isSel ? GOLD : GRAY);

        // Draw tile preview
        char tChar = availableTiles[i].type;
        std::string charStr(1, tChar);
        DrawText(charStr.c_str(), bx + 12, by + 10, 20, availableTiles[i].color);
    }

    // Grid Viewport
    int startY = 70;
    for (int r = 0; r < gridHeight; ++r) {
        for (int c = 0; c < gridWidth; ++c) {
            float screenX = c * 32.0f - cameraX;
            float screenY = startY + r * 32.0f;

            if (screenX + 32.0f < 0 || screenX > GetScreenWidth()) continue;

            // Draw tile box grid
            DrawRectangleLines((int)screenX, (int)screenY, 32, 32, Color{ 45, 45, 55, 255 });

            char ch = grid[r][c];
            if (ch != '.') {
                Color tileCol = DARKGRAY;
                for (const auto& t : availableTiles) {
                    if (t.type == ch) {
                        tileCol = t.color;
                        break;
                    }
                }
                DrawRectangle((int)screenX + 2, (int)screenY + 2, 28, 28, tileCol);
                std::string s(1, ch);
                DrawText(s.c_str(), (int)screenX + 10, (int)screenY + 6, 16, WHITE);
            }
        }
    }

    // Highlight hovered cell under mouse
    Vector2 mPos = GetMousePosition();
    if (mPos.y > 70 && mPos.y < GetScreenHeight() - 60) {
        int hoverC = (int)((mPos.x + cameraX) / 32.0f);
        int hoverR = (int)((mPos.y - 70.0f) / 32.0f);
        if (hoverR >= 0 && hoverR < gridHeight && hoverC >= 0 && hoverC < gridWidth) {
            float hX = hoverC * 32.0f - cameraX;
            float hY = startY + hoverR * 32.0f;
            DrawRectangleLinesEx(Rectangle{ hX, hY, 32.0f, 32.0f }, 2.0f, YELLOW);
        }
    }

    // Bottom Controls & Status Bar
    DrawRectangle(0, GetScreenHeight() - 60, GetScreenWidth(), 60, Color{ 25, 25, 35, 255 });
    
    std::string activeTileInfo = "Selected: " + availableTiles[selectedTileIndex].name + " (" + availableTiles[selectedTileIndex].type + ")";
    DrawText(activeTileInfo.c_str(), 20, GetScreenHeight() - 48, 18, SKYBLUE);

    if (statusTimer > 0.0f) {
        DrawText(statusMessage.c_str(), 320, GetScreenHeight() - 48, 16, GOLD);
    } else {
        DrawText("[S] Save  |  [T] Test Play Level  |  [ESC] Main Menu", 350, GetScreenHeight() - 48, 16, LIGHTGRAY);
    }
}
