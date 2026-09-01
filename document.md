# Super Mario Replica — AI Continuity & Architectural Reference

> **Purpose**: This document provides a complete architectural reference for the project so that any AI assistant (or developer) can understand the full codebase structure and continue building or expanding the game without ambiguity.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Technology Stack & Build System](#2-technology-stack--build-system)
3. [Directory Structure](#3-directory-structure)
4. [Architecture & Design Patterns](#4-architecture--design-patterns)
5. [Module Reference: Core](#5-module-reference-core)
6. [Module Reference: Entities](#6-module-reference-entities)
7. [Module Reference: Physics](#7-module-reference-physics)
8. [Module Reference: World](#8-module-reference-world)
9. [Module Reference: Persistence](#9-module-reference-persistence)
10. [Module Reference: States (Game Screens)](#10-module-reference-states-game-screens)
11. [Entity Inheritance Hierarchy](#11-entity-inheritance-hierarchy)
12. [Multiple Character System (Mario vs. Luigi)](#12-multiple-character-system-mario-vs-luigi)
13. [In-Game Level Editor](#13-in-game-level-editor)
14. [Game State Flow](#14-game-state-flow)
15. [Level File Format](#15-level-file-format)
16. [Account Serialization Format](#16-account-serialization-format)
17. [Asset Manifest](#17-asset-manifest)
18. [Event System Reference](#18-event-system-reference)
19. [Key Constants & Physics Values](#19-key-constants--physics-values)
20. [How to Extend the Game](#20-how-to-extend-the-game)

---

## 1. Project Overview

A 2D Mario-style platformer built in **C++17** using **Raylib 5.0** for rendering and audio. The game features:

- State-stack-based screen management (login, menus, gameplay, pause, settings, level editor)
- Independent-axis physics with predictive collision detection and MTV push-out recovery
- Player power-up state machine (Small → Super → Fire)
- Multiple playable characters (Mario vs Luigi with distinct jump physics & speeds)
- Interactive in-game Level Editor with live test-play capabilities
- 3 distinct prebuilt levels (`level1.txt`, `level2.txt`, `level3.txt`) + Custom Level support (`custom_level.txt`)
- Enemy AI via strategy pattern (Patrol, Slam, State machines)
- Observer-based event system for decoupled audio/scoring
- Local account persistence with key rebinding & character preference saving
- Asset fallback system (debug rectangles when sprites are missing)

---

## 2. Technology Stack & Build System

| Component | Technology |
|---|---|
| Language | C++17 |
| Graphics/Audio | Raylib 5.0 (fetched via CMake FetchContent) |
| Build System | CMake 3.12+ |
| Window Size | 800×600 at 60 FPS |
| Target Platform | Windows (with IME disabling) |

### Build Commands

```powershell
mkdir build
cd build
cmake .. "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
.\Release\MarioGame.exe
```

### CMake Notes
- Raylib is automatically downloaded and compiled from source via `FetchContent`
- Source files are gathered recursively with `file(GLOB_RECURSE)` from `src/`
- Assets are copied post-build to the output directory
- The `accounts/` directory is created in the output directory post-build

---

## 3. Directory Structure

```
CS202/
├── CMakeLists.txt                 # CMake config — fetches Raylib, gathers sources
├── main.cpp                       # Entry point — creates GameEngine singleton, runs game loop
├── requirement.md                 # Project requirement specifications
├── document.md                    # AI continuity and architecture reference
├── accounts/                      # User profile save files (.txt)
│   ├── Guest.txt
│   ├── TestUser2.txt
│   └── TestUser3.txt
├── assets/
│   ├── audio/                     # SFX (.wav) and BGM (.wav) files
│   ├── textures/                  # PNG sprite sheets / images
│   └── levels/                    # Text-grid level layout files
│       ├── level1.txt             # Overworld Grasslands
│       ├── level2.txt             # Underground Cavern
│       ├── level3.txt             # Bowser's Fortress
│       └── custom_level.txt       # User Level Editor output
└── src/
    ├── Core/                      # Engine, asset/sound managers, input, events, state management
    │   ├── AssetManager.h/.cpp
    │   ├── EventSystem.h
    │   ├── GameEngine.h/.cpp
    │   ├── GameState.h
    │   ├── GameStateManager.h/.cpp
    │   ├── InputManager.h/.cpp
    │   └── SoundManager.h/.cpp
    ├── Entities/                   # All game entities (player, enemies, blocks, items, projectiles)
    │   ├── Entity.h/.cpp           # Abstract base class
    │   ├── DynamicEntity.h/.cpp    # Moving entities (velocity, gravity)
    │   ├── StaticEntity.h/.cpp     # Non-moving entities (blocks)
    │   ├── Player.h/.cpp           # Player controller (Mario / Luigi support)
    │   ├── PlayerPowerState.h/.cpp # Small/Super/Fire state machine
    │   ├── SpecialMove.h/.cpp      # Fireball/None strategy
    │   ├── Fireball.h/.cpp         # Player projectile
    │   ├── Enemy.h/.cpp            # Enemy base + AI strategy
    │   ├── Goomba.h/.cpp           # Walking enemy
    │   ├── Koopa.h/.cpp            # Shell enemy (walk/shell/slide states)
    │   ├── PiranhaPlant.h/.cpp     # Pipe-dwelling enemy
    │   ├── RockHead.h/.cpp         # Thwomp-style crusher
    │   ├── Block.h/.cpp            # Solid ground block
    │   ├── InteractiveBlock.h/.cpp # Brick & question blocks
    │   └── Item.h/.cpp             # Collectible items (mushroom, flower, star)
    ├── Physics/
    │   ├── Camera.h/.cpp           # Horizontal-follow camera with boundary clamping
    │   └── CollisionManager.h/.cpp # AABB look-ahead + MTV push-out collision resolver
    ├── World/
    │   ├── EntityFactory.h/.cpp    # Char → Entity factory for level parsing
    │   └── Level.h/.cpp            # Level loader, entity manager, update/render loop
    ├── Persistence/
    │   ├── Account.h/.cpp          # User profile data model (supports character selection)
    │   └── SaveManager.h/.cpp      # File I/O for accounts
    └── States/                     # Game screen implementations
        ├── LoginState.h/.cpp
        ├── RegisterState.h/.cpp
        ├── MainMenuState.h/.cpp
        ├── LevelSelectState.h/.cpp
        ├── LevelEditorState.h/.cpp  # In-game level designer state
        ├── GameplayState.h/.cpp
        ├── PauseState.h/.cpp
        ├── SettingsState.h/.cpp
        └── GameOverState.h/.cpp
```

---

## 4. Architecture & Design Patterns

### 4.1 Singleton Pattern
Used by the following globally-accessible managers:
- **`GameEngine`** — window lifecycle, game loop, owns state manager and input manager
- **`AssetManager`** — texture cache (prevents duplicate GPU loads)
- **`SoundManager`** — audio loading/playback + event-driven SFX
- **`EventManager`** — central event bus for decoupled communication

### 4.2 State Pattern
- **Game States**: `GameStateManager` uses a `std::vector<GameState*>` as a stack. States can be pushed (overlay), popped, or switched. State mutations are deferred via a pending action queue to avoid iterator invalidation.
- **Player Power States**: `Player` holds a `PlayerPowerState*` pointer. Concrete states (`SmallState`, `SuperState`, `FireState`) define movement speed, hitbox dimensions, damage response, and abilities.

### 4.3 Strategy Pattern
- **AI Behavior**: `Enemy` holds an `AIBehaviorStrategy*`. Concrete: `PatrolHorizontalStrategy` (walk back and forth). PiranhaPlant and RockHead use `nullptr` (custom internal state machines).
- **Special Moves**: `Player` holds a `std::unique_ptr<SpecialMove>`. Concrete: `FireballMove` (shoots fireball) and `NoneMove` (no-op for Small/Super).

### 4.4 Observer Pattern
- `EventManager` dispatches `EventType` events to registered `EventListener*` subscribers.
- `SoundManager` subscribes to play SFX on game events.
- Entities broadcast events without direct coupling to audio or scoring.

### 4.5 Factory Method Pattern
- `EntityFactory::createEntity(char, float, float)` maps text characters to entity subclass instances.

### 4.6 Deferred Command Pattern
- `GameStateManager` queues state transitions (`PUSH`, `POP`, `CHANGE`, `CLEAR`) and processes them between frames to avoid stack corruption.

---

## 5. Module Reference: Core

### 5.1 GameEngine (`Core/GameEngine.h/.cpp`)
**Singleton** — top-level application facade.

| Member | Type | Description |
|---|---|---|
| `stateManager` | `GameStateManager` | Owns the state stack |
| `inputManager` | `InputManager` | Owns the input mapping system |
| `activeAccount` | `Account` | Currently logged-in user profile |
| `isRunning` | `bool` | Game loop flag |

**Key methods:**
- `init()` — Creates 800×600 window, 60 FPS, disables Windows IME, initializes audio, preloads all textures & sounds, pushes initial `MainMenuState`
- `run()` — Main loop: `processPendingActions()` → `handleInput()` → `update(dt)` (dt capped at 0.1f) → `SoundManager.update()` → `BeginDrawing/ClearBackground(SKYBLUE)/draw()/EndDrawing`
- `cleanup()` — Clears states, assets, audio, closes window
- `exitGame()` — Sets `isRunning = false`
- `getStateManager()` / `getInputManager()` / `getActiveAccount()` / `setActiveAccount()` — Accessors

### 5.2 AssetManager (`Core/AssetManager.h/.cpp`)
**Singleton** — GPU texture cache.

| Method | Description |
|---|---|
| `loadTexture(id, filePath)` | Loads `.png` via Raylib. Returns `false` if load fails (`tex.id == 0`) |
| `getTexture(id)` | Returns cached `Texture2D` or blank fallback `{0,0,0,0,0}` |
| `unloadTexture(id)` | Frees GPU memory for one texture |
| `clear()` | Unloads all textures |

### 5.3 EventSystem (`Core/EventSystem.h`)
Header-only. Defines:

**`enum class EventType`:**
`PlayerJump`, `PlayerHurt`, `PlayerDied`, `CoinCollected`, `EnemyStomped`, `BrickBroken`, `LevelCompleted`, `FireballShot`, `ItemSpawned`

**`EventListener` (abstract interface):**
- `virtual void onEvent(EventType type, void* data = nullptr) = 0`

**`EventManager` (singleton):**
- `subscribe(EventType, EventListener*)` — Register a listener
- `unsubscribe(EventType, EventListener*)` — Remove a listener
- `broadcast(EventType, void* data)` — Notify all subscribers

### 5.4 GameState (`Core/GameState.h`)
Abstract interface for all screens:
```cpp
virtual void init() = 0;
virtual void handleInput(const InputManager& input) = 0;
virtual void update(float dt) = 0;
virtual void draw() = 0;
virtual void onBack() {}  // Default: no-op on ESC
```

### 5.5 GameStateManager (`Core/GameStateManager.h/.cpp`)
Manages a `std::vector<GameState*>` stack with deferred transitions.

### 5.6 InputManager (`Core/InputManager.h/.cpp`)
Maps `Action` enum values to Raylib key codes.

### 5.7 SoundManager (`Core/SoundManager.h/.cpp`)
**Singleton + EventListener** — audio subsystem.

---

## 6. Module Reference: Entities

### 6.1 Entity (`Entities/Entity.h/.cpp`)
**Abstract base** for all game objects.

### 6.2 DynamicEntity (`Entities/DynamicEntity.h/.cpp`)
**Abstract** — extends Entity with physics (`velocity`, `gravity`, `CollisionSide`).

### 6.3 StaticEntity (`Entities/StaticEntity.h/.cpp`)
**Abstract** — for non-moving entities (`solid`).

### 6.4 Player (`Entities/Player.h/.cpp`)
Central player entity with support for `CharacterType::Mario` and `CharacterType::Luigi`.

| Attribute | Mario | Luigi |
|---|---|---|
| Jump Force (`jumpForce`) | 420.0f px/s | **480.0f px/s** (Higher jump!) |
| Movement Speed (`speed`) | 250.0f px/s | 230.0f px/s (Slower run) |
| Texture ID | `"mario"` | `"luigi"` (falls back to `"mario"`) |
| Fallback Tint / Color | `RED` | **`GREEN`** (`Color{120, 255, 120, 255}`) |

---

## 7. Module Reference: Physics

### 7.1 Camera (`Physics/Camera.h/.cpp`)
Follows player horizontally, clamped to level bounds.

### 7.2 CollisionManager (`Physics/CollisionManager.h/.cpp`)
AABB look-ahead prediction + MTV push-out stuck recovery.

---

## 8. Module Reference: World

### 8.1 EntityFactory (`World/EntityFactory.h/.cpp`)
Character-to-entity map (`#`, `B`, `?`, `M`, `F`, `S`, `G`, `K`, `T`, `I`).

### 8.2 Level (`World/Level.h/.cpp`)
Level file parser, entity manager, player spawner (reads selected character from active Account), viewport updater.

---

## 9. Module Reference: Persistence

### 9.1 Account (`Persistence/Account.h/.cpp`)
User profile data model storing username, password hash, max unlocked level, high scores, key settings, and `selectedCharacter` ("Mario" or "Luigi").

### 9.2 SaveManager (`Persistence/SaveManager.h/.cpp`)
Reads and writes account files to `accounts/<username>.txt` including key bindings, level scores, and `CHARACTER` preference lines.

---

## 10. Module Reference: States (Game Screens)

1. `LoginState` — Credential verification
2. `RegisterState` — Account creation
3. `MainMenuState` — Play, Character Selection (Mario/Luigi toggle), Level Editor, Login, Register, Settings, Quit
4. `LevelSelectState` — Selection grid for Level 1, Level 2, Level 3, and Custom Level
5. `LevelEditorState` — Grid canvas tile placement, tile palette, saving, and instant test-playing
6. `GameplayState` — Active gameplay level ticker
7. `PauseState` — Transparent overlay pause screen
8. `SettingsState` — Key rebinding
9. `GameOverState` — Victory/Defeat screen

---

## 11. Entity Inheritance Hierarchy

```
Entity (abstract)
├── DynamicEntity (abstract)
│   ├── Player (Mario or Luigi)
│   ├── Enemy (abstract)
│   │   ├── Goomba
│   │   ├── Koopa
│   │   ├── PiranhaPlant
│   │   └── RockHead
│   ├── Fireball
│   └── Item
└── StaticEntity (abstract)
    ├── Block (solid ground / boundary wall)
    ├── InteractiveBlock (brick / question block)
    └── ExitBlock (Warp Pipe / Goal Door level exit)
```

---

## 12. Multiple Character System (Mario vs Luigi)

Players can toggle their preferred character in the **Main Menu** ("CHARACTER: MARIO" / "CHARACTER: LUIGI"):

- **Mario**: Standard platformer balance (250 speed, 420 jump impulse).
- **Luigi**: Higher jumping capability (480 jump impulse) with slightly reduced speed (230 speed) and green tint/sprite styling.
- **Persistence**: Selected character preference is serialized to disk (`CHARACTER Mario` / `CHARACTER Luigi`) per user profile in `SaveManager`.

---

## 13. In-Game Level Editor

Accessible directly from the **Main Menu** via "LEVEL EDITOR":

- **Grid Canvas**: 100 columns × 15 rows tile grid.
- **Controls**:
  - `WASD` / `Arrow Keys`: Scroll camera horizontally.
  - Number keys `0`-`9` / `E`: Quick-select tile palette item (Player spawn `P`, Block `#`, Brick `B`, Question Coin `?`, Mushroom `M`, FireFlower `F`, Goomba `G`, Koopa `K`, Thwomp `T`, Piranha Plant `I`, Exit Warp Pipe `W`, Eraser `.`).
  - Left Mouse Click: Place tile cell.
  - Right Mouse Click: Erase cell.
  - `S` key: Save custom map to `assets/levels/custom_level.txt`.
  - `T` key: Test play custom map immediately in `GameplayState`.

---

## 14. Game State Flow

```
                               ┌──────────────┐
                               │  LoginState  │
                               └──────┬───────┘
                             Login/   │   \Guest
                           Register   │
                      ┌───────┐       │
                      │Register│      │
                      │ State  │──────┘
                      └────────┘
                               │
                       ┌───────▼──────┐
                       │ MainMenuState│
                       └──┬─┬──┬──┬───┘
            Play Game     │ │  │  │ Level Editor
          ┌───────────────┘ │  │  └───────────────┐
          │     Toggle Char │  │ Settings         │
          │  ┌──────────────┘  └────────┐         │
          │  │                          │         │
   ┌──────▼──▼───┐              ┌───────▼─────┐ ┌─▼──────────────┐
   │ LevelSelect │              │SettingsState│ │LevelEditorState│
   │    State    │              └─────────────┘ └────────────────┘
   └──────┬──────┘
          │ Select level
   ┌──────▼──────┐
   │GameplayState│◄───── Resume
   └──┬───────┬──┘
Pause │       │ Win/Lose
┌─────▼────┐  │
│PauseState │  │
└─────┬─────┘  │
 Quit │   ┌────▼─────┐
 to   │   │GameOver  │
 Menu │   │  State   │
      │   └──────────┘
      └────────┘
```

---

## 15. Level Modes & Level File Format

The game supports **three distinct level modes**:
1. **Prebuilt Handcrafted Levels**: Text files in `assets/levels/` (`level1.txt`, `level2.txt`, `level3.txt`).
2. **In-Game Custom Level Editor**: Created and edited directly by the player (`custom_level.txt`).
3. **Procedural Random Level Generator**: Selecting the **RANDOM** card procedurally generates a brand-new level layout on demand with randomized platforming obstacles, item question blocks, enemy waves, ground pit gaps, and an Exit Warp Pipe!

**Character legend:**

| Char | Entity |
|---|---|
| `#` | Solid ground / boundary wall block |
| `B` | Breakable brick block |
| `?` | Question block (coin) |
| `M` | Question block (mushroom) |
| `F` | Question block (fire flower) |
| `S` | Question block (star) |
| `G` | Goomba spawn |
| `K` | Koopa spawn |
| `T` | RockHead / Thwomp spawn |
| `I` | Piranha Plant spawn |
| `W` / `E` | Exit Warp Pipe / Goal Door (touch/enter to advance to next level) |
| `P` | Player spawn position |
| `.` | Empty space |

---

## 16. Account Serialization Format

Stored in `accounts/<username>.txt`:

```
<username>
<passwordHash>
<currentLevel>
<highScore>
CHARACTER <Mario|Luigi>
LEVEL_SCORE <index> <score>
KEY <actionName> <keyCode>
```

---

## 17. Asset Manifest

### Textures (`assets/textures/`)
`mario.png`, `luigi.png`, `goomba.png`, `koopa.png`, `koopa_shell.png`, `piranha.png`, `thwomp.png`, `solid.png`, `brick.png`, `question.png`, `mushroom.png`, `flower.png`, `star.png`, `coin.png`, `fireball.png`, `mainmenu.png`.

### Audio (`assets/audio/`)
`jump.wav`, `stomp.wav`, `coin.wav`, `hurt.wav`, `die.wav`, `break.wav`, `menu_theme.wav`, `level_theme.wav`.

---

## 18. Event System Reference

| EventType | Producer | Consumer |
|---|---|---|
| `PlayerJump` | `Player::jump()` | `SoundManager` |
| `PlayerHurt` | `PlayerPowerState` | `SoundManager` |
| `PlayerDied` | `SmallState::onDamage()` | `SoundManager`, `GameplayState` |
| `CoinCollected` | `InteractiveBlock`, `Item` | `SoundManager` |
| `EnemyStomped` | `Enemy`, `Koopa` | `SoundManager` |
| `BrickBroken` | `InteractiveBlock` | `SoundManager` |
| `LevelCompleted` | `Level` | `GameplayState` |
| `FireballShot` | `Player::shootFireball()` | `Level` / `GameplayState` |
| `ItemSpawned` | `InteractiveBlock` | `Level` / `GameplayState` |

---

## 19. Key Constants & Physics Values

| Constant | Value |
|---|---|
| `TILE_SIZE` | 32.0f px |
| `gravity` | 800.0f px/s² |
| `terminalVelocity` | 500.0f px/s |
| Mario `speed` / `jumpForce` | 250.0f / 420.0f |
| Luigi `speed` / `jumpForce` | 230.0f / 480.0f |
| Window resolution | 800×600 @ 60 FPS |

---

## 20. How to Extend the Game

Refer to section guidelines above for creating new entity subclasses, game states, AI strategies, or serializable user settings.
