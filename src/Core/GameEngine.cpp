#include "GameEngine.h"
#include "raylib.h"
#include "SoundManager.h"
#include "AssetManager.h"
#include "States/MainMenuState.h"
#include <iostream>

#ifdef _WIN32
// Temporarily rename Windows API symbols to prevent collisions with Raylib struct/functions
#define Rectangle Win32Rectangle
#define CloseWindow Win32CloseWindow
#define ShowCursor Win32ShowCursor
#include <windows.h>
// Restore original Raylib symbols
#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#endif

GameEngine::GameEngine() : isRunning(true), isInitialized(false) {}

GameEngine::~GameEngine() {
    cleanup();
}

void GameEngine::init() {
    if (isInitialized) return;
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Super Mario OOP C++ - Raylib");
    SetTargetFPS(60);
    disableWindowIME();

    // Initialize Audio device and event subscriptions
    SoundManager::getInstance().init();

    // Check directory folders
    // Create local directories if missing (will be handled by file structure, but verify)
    // Try to load standard sound assets (optional: falls back if missing)
    SoundManager::getInstance().loadSound("jump", "assets/audio/jump.wav");
    SoundManager::getInstance().loadSound("stomp", "assets/audio/stomp.wav");
    SoundManager::getInstance().loadSound("coin", "assets/audio/coin.wav");
    SoundManager::getInstance().loadSound("hurt", "assets/audio/hurt.wav");
    SoundManager::getInstance().loadSound("die", "assets/audio/die.wav");
    SoundManager::getInstance().loadSound("break", "assets/audio/break.wav");

    // Try to load textures
    AssetManager::getInstance().loadTexture("mario", "assets/textures/Mario.png");
    AssetManager::getInstance().loadTexture("goomba", "assets/textures/goomba.png");
    AssetManager::getInstance().loadTexture("koopa", "assets/textures/koopa.png");
    AssetManager::getInstance().loadTexture("piranha", "assets/textures/piranha.png");
    AssetManager::getInstance().loadTexture("thwomp", "assets/textures/thwomp.png");
    AssetManager::getInstance().loadTexture("brick", "assets/textures/brick.png");
    AssetManager::getInstance().loadTexture("solid", "assets/textures/solid.png");
    AssetManager::getInstance().loadTexture("question", "assets/textures/question.png");
    AssetManager::getInstance().loadTexture("mushroom", "assets/textures/mushroom.png");
    AssetManager::getInstance().loadTexture("flower", "assets/textures/flower.png");
    AssetManager::getInstance().loadTexture("star", "assets/textures/star.png");
    AssetManager::getInstance().loadTexture("coin", "assets/textures/coin.png");
    AssetManager::getInstance().loadTexture("fireball", "assets/textures/fireball.png");

    // Load initial screen state
    SetExitKey(KEY_NULL); // Prevent ESC key from closing window in Raylib
    isInitialized = true;
    stateManager.pushState(new MainMenuState());
}

void GameEngine::run() {
    while (isRunning && !WindowShouldClose()) {
        stateManager.processPendingActions();

        // 1. Input Processing
        inputManager.update();
        stateManager.handleInput(inputManager);

        // 2. Physics & Logic Update
        float dt = GetFrameTime();
        // Cap dt to prevent massive jumps when dragging window
        if (dt > 0.1f) dt = 0.1f;

        stateManager.update(dt);
        SoundManager::getInstance().update(); // Update streaming music streams

        // 3. Frame Rendering
        BeginDrawing();
        ClearBackground(SKYBLUE);

        stateManager.draw();

        EndDrawing();
    }
}

void GameEngine::cleanup() {
    if (!isInitialized) return;
    stateManager.clear();
    AssetManager::getInstance().clear();
    SoundManager::getInstance().cleanup();
    CloseWindow();
    isInitialized = false;
}

void GameEngine::disableWindowIME() {
#ifdef _WIN32
    // Safely attempt dynamic loading of imm32.dll
    HMODULE hImm32 = LoadLibraryA("imm32.dll");
    if (hImm32) {
        typedef BOOL (WINAPI *ImmDisableIMEPtr)(DWORD);
        typedef HIMC (WINAPI *ImmAssociateContextPtr)(HWND, HIMC);

        // Cast through void* to suppress GCC incompatible function pointer cast warning
        ImmDisableIMEPtr pImmDisableIME = (ImmDisableIMEPtr)(void*)GetProcAddress(hImm32, "ImmDisableIME");
        ImmAssociateContextPtr pImmAssociateContext = (ImmAssociateContextPtr)(void*)GetProcAddress(hImm32, "ImmAssociateContext");

        if (pImmDisableIME) {
            pImmDisableIME(-1); // Disable IME for all threads of this process
        }

        HWND hwnd = (HWND)GetWindowHandle();
        if (hwnd && pImmAssociateContext) {
            pImmAssociateContext(hwnd, NULL); // Dissociate IME context for the game window
        }
    }
#endif
}
