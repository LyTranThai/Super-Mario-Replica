#ifndef LEVEL_EDITOR_STATE_H
#define LEVEL_EDITOR_STATE_H

#include "Core/GameState.h"
#include "Physics/Camera.h"
#include <vector>
#include <string>
#include "raylib.h"

struct EditorTile {
    char type;
    std::string name;
    Color color;
};

class LevelEditorState : public GameState {
private:
    std::vector<std::string> grid;
    int gridWidth;
    int gridHeight;
    float cameraX;
    int selectedTileIndex;
    std::vector<EditorTile> availableTiles;
    std::string statusMessage;
    float statusTimer;

public:
    LevelEditorState();
    ~LevelEditorState() override = default;

    void init() override;
    void handleInput(const InputManager& input) override;
    void update(float dt) override;
    void draw() override;
    void onBack() override;

private:
    void saveCustomLevel();
    void playCustomLevel();
};

#endif // LEVEL_EDITOR_STATE_H
