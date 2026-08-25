#include "Core/GameEngine.h"
#include <cstdio>
#include "raylib.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

int main() {
    // Force working directory to the project root instead of the build directory
#ifdef PROJECT_ROOT_DIR
    ChangeDirectory(PROJECT_ROOT_DIR);
#endif

    // std::freopen("debug.txt", "w", stdout);
    // std::freopen("debug.txt", "w", stderr);
    
    GameEngine& engine = GameEngine::getInstance();
    engine.init();
    GameEngine::getInstance().run();
    GameEngine::getInstance().cleanup();
    return 0;
}
