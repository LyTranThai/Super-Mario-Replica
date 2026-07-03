# C++ OOP Mario Game (Built with Raylib)

A 2D Mario-style platformer game implemented in C++17 utilizing Object-Oriented Programming (OOP) principles and structured software design patterns. It handles sprite rendering, custom kinematics, predictive collision detection, state-stack loops, account serialization, and event-driven audio/score updates.

---

## 1. Core Features & Capabilities

- **Game Screens & Flow (State Stack)**: Pushes and pops game state overlays (`MainMenu`, `Gameplay`, `Settings`, `Pause`, `GameOver`). Freezes gameplay updates during transparent pauses.
- **Independent Axis Physics & Kinematics**:
  - Independent 2D vectors (`Vector2`) calculate vertical gravity and horizontal speed separate from each other to ensure responsive movement weight.
  - Instantly stops and accelerates based on inputs (muscle memory feel) and caps vertical fall speed with a `terminalVelocity` ceiling to prevent solid block clipping.
  - Mid-air **Double Jump** tracked via jump counters.
- **Fair-play Collision Manager (Look-Ahead & Recovery)**:
  - **Predictive Look-Ahead**: Separately checks projected horizontal and vertical coordinates relative to velocity vectors and delta times. Snaps vertical coordinates to solid ground tops and cancels sliding.
  - **Stuck Recovery (Push-out fallback)**: Evaluates Minimum Translation Vector overlays to push out any clipped entities along their shallowest axis.
  - **Solid Filtering**: Selectively checks collisions (e.g. allows emerging mushrooms or piranha plant pipe movements to bypass solid collisions temporarily).
  - **Visual vs. Hitbox Separation**: Hitboxes (`hitboxSize` and `hitboxOffset`) are decoupled and slightly smaller than visual sizes (`spriteSize`) for a lenient and forgiving gaming experience.
- **Asset Fallback System**: If custom sprites (`.png`) or audio files (`.wav`) are missing from `/assets/`, the engine draws colored debug rectangles and bypasses audio checks, preventing runtime crashes.
- **User Accounts and Progress Saving**: Local data persistence serializes profiles (key configuration, levels unlocked, and high scores) into simple text streams inside the `accounts/` directory.
- **Decoupled Settings**: Rebind gameplay keys through the main menu settings screen, which automatically saves config settings to user profile text files.

---

## 2. Design Patterns Implemented

1. **Singleton Pattern**:
   - `GameEngine`: Maintains window lifespan, game loops, and initializations.
   - `AssetManager`: Caches loaded sprite textures to prevent double-allocation.
   - `SoundManager`: Audio buffer loader and playbacks.
   - `EventManager`: Dispatches game-wide global events.
2. **State Pattern**:
   - **Screens**: `GameStateManager` uses a `std::stack<GameState*>` to switch/overlay `MainMenuState`, `GameplayState`, `PauseState`, and `GameOverState`.
   - **Powerups**: The player holds a `PlayerPowerState*` pointer. Calls to update, render, or damage are delegated to concrete subclasses: `SmallState` (default), `SuperState` (height doubled, break bricks), and `FireState` (can shoot bouncy fireballs).
3. **Factory Method Pattern**:
   - `EntityFactory::createEntity(char type, float x, float y)` parses text character matrix layouts (e.g., `#` for ground blocks, `?` for interactive items, `G` for Goombas) to instantiate subclass instances dynamically.
4. **Observer Pattern**:
   - `EventManager` acts as a decoupled central message bus. Entities broadcast actions (`PlayerDied`, `CoinCollected`, `PointsEarned`), and subscribed handlers (`SoundManager`, scoring trackers) respond without direct cross-class coupling.
5. **Strategy Pattern**:
   - `AIBehaviorStrategy` decouples intelligence updates from concrete `Enemy` classes. Subclasses customize movement loops (e.g., `PatrolStrategy` for standard walking Goombas, `SlamStrategy` for vertical RockHead drops).

---

## 3. Folder Structure

The implementation is structured within the `1st_src/` folder as follows:

```text
1st_src/
├── CMakeLists.txt              # CMake configuration for Raylib fetching and target builds
├── main.cpp                     # Graphical game launcher entry point
├── test_mario.cpp               # Headless unit-testing suite
├── test.cpp                     # Diagnostic sanity check program
├── draft4.md                    # Agreed system design specifications
├── accounts/                    # Folder where user profile files are serialized
└── assets/                      # External resource folders
    ├── audio/                   # SFX and Background Music tracks
    ├── textures/                # PNG sheets for sprites
    └── levels/                  # Text-based grid layout maps (level1.txt, etc.)
        └── readme.txt
```

### Source Code Module (`src/`)

```text
src/
├── Core/
│   ├── AssetManager.h / .cpp    # Cache loaders for graphic assets
│   ├── EventSystem.h            # Central Observer EventManager and listeners
│   ├── GameEngine.h / .cpp      # Core system updater and frame ticker
│   ├── GameState.h              # GameState abstract class
│   ├── GameStateManager.h/.cpp  # Screen transitions stack administrator
│   ├── InputManager.h / .cpp    # Action rebindings interface mapping keys
│   └── SoundManager.h / .cpp    # BGM loop queues and SFX trigger listener
├── Entities/
│   ├── Entity.h / .cpp          # Core base containing bounds, draw checks, and fallback box rendering
│   ├── DynamicEntity.h / .cpp   # Subclass adding velocity, gravity, and facing directions
│   ├── Player.h / .cpp          # Player entity tracking stats, jump count, and power status
│   ├── PlayerPowerState.h/.cpp  # State machine delegation (Small, Super, Fire states)
│   ├── Fireball.h / .cpp        # Projectile tracking bouncing physics
│   ├── Enemy.h / .cpp           # Base class containing strategic AI controllers
│   │   ├── Goomba.h / .cpp      # Flat-stomped walking enemy
│   │   ├── Koopa.h / .cpp       # Kickable sliding shell enemy
│   │   ├── PiranhaPlant.h/.cpp  # Vertical emerging pipe-dweller
│   │   └── RockHead.h / .cpp    # Vertical dropping crusher
│   ├── StaticEntity.h / .cpp    # Base class for stationary blocks
│   │   ├── Block.h / .cpp       # Immovable solid ground
│   │   └── InteractiveBlock.h   # Breakable brick blocks and item spawner blocks
│   └── Item.h / .cpp            # Mushrooms, Fire Flowers, Stars, and Coins
├── Persistence/
│   ├── Account.h / .cpp         # Credentials, settings, and progress model
│   └── SaveManager.h / .cpp     # File streams serializer writing to accounts/
├── Physics/
│   ├── Camera.h / .cpp          # GameCamera clamped to level horizontal bounds
│   └── CollisionManager.h/.cpp  # Predictor axis checker and MTV push-out solver
├── States/
│   ├── MainMenuState.h / .cpp   # Login/Register UI, level selector, and settings rebind overlays
│   ├── GameplayState.h / .cpp   # Level updates loop and camera viewport ticker
│   ├── PauseState.h / .cpp      # Transparent screen freeze overlay menu
│   └── GameOverState.h / .cpp   # Victory/Defeat score banners
└── World/
    ├── EntityFactory.h / .cpp   # Matrix grid spawner translates chars to Entities
    └── Level.h / .cpp           # Keeps entities list, updates viewport, and drives collisions
```

---

## 4. How to Build and Run

### Prerequisites
- **CMake** (version 3.12 or higher)
- **C++17 Compiler** (MSVC, MinGW-W64 `g++`, or Clang)
- **Git** (required by CMake to fetch Raylib from source)

### Building instructions

1. Create a `build` directory inside `1st_src/` and configure the project:
   ```powershell
   cd 1st_src
   mkdir build
   cd build
   cmake .. "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
   ```
   *(Note: The `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` compiler policy override is required on CMake 4.0+ due to Raylib subprojects requiring CMake version overrides)*

2. Build the project targets:
   ```powershell
   cmake --build . --config Release
   ```
   This compiles two primary binaries:
   - `MarioGame.exe`: The primary playable game client.
   - `TestMario.exe`: A headless test suite validating persistence and kinematics.

3. Run the executables:
   - **Start the game**:
     ```powershell
     ./MarioGame.exe
     ```
   - **Run the test suites**:
     ```powershell
     ./TestMario.exe
     ```

---

## 5. Game Controls

### Default Keyboard Controls:
- **Move Left / Right**: `A` / `D` or `Left Arrow` / `Right Arrow`
- **Jump / Double Jump**: `Space` or `W` or `Up Arrow`
- **Shoot Fireball**: `F` or `Left Shift` (when in Fire state)
- **Pause Game**: `P` or `Escape`
- **Settings Rebinding**: Accessible from the Settings menu in `MainMenuState` or `PauseState` to customize key setups.
