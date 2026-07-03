#include "Core/GameEngine.h"
#include <cstdio>

int main() {
    std::freopen("debug.txt", "w", stdout);
    std::freopen("debug.txt", "w", stderr);
    
    GameEngine& engine = GameEngine::getInstance();
    engine.init();
    GameEngine::getInstance().run();
    GameEngine::getInstance().cleanup();
    return 0;
}
