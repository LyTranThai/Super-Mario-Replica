# CS Mario Game - Fourth Draft Design (draft4.md)

This is the consolidated blueprint of the OOP C++ Mario Game designed with Raylib. It compiles all system components, physics, collision math, design patterns, and entity classes agreed upon for implementation.

---

## 1. Core Game Features
*   **3 Progression Levels**: Levels built with rising layout challenges and enemy configurations.
*   **Camera Viewport Scrolling**: Camera centered horizontally on the player, bound within the level limits.
*   **Game Pause**: State-stack system that freezes active game frames and displays a pause overlay menu (Resume, Settings, Main Menu).
*   **Data Persistence (Accounts)**: Serializes user profiles (unlocked levels, high scores, custom controls) to text files under the `accounts/` directory.
*   **Audio Controller**: Singletons handling loop tracks (BGM) and spatial/one-shot sound effects (jump, stomp, coin, damage).

---

## 2. Physics & Kinematics Specification
*   **Independent Axes**: Velocity and acceleration use 2D vectors (`Vector2`). Horizontal and vertical calculations are independent to preserve player muscle memory.
*   **Gravity & Terminal Velocity**: Constant downward acceleration: `velocity.y += gravity * dt`. Capped at `terminalVelocity` to prevent clipping bugs.
*   **Action Responsiveness**: Instant starting speed (immediate setting of maximum speed) and instant damping (zeros velocity instantly on key release).
*   **Double Jump**: Tracked via `Player::jumpCount`, allowing one mid-air jump. Resets when hitting solid ground.

---

## 3. Rendering & Hitbox Design (Fallback Box)
*   **Visual vs. Collision Boundaries**:
    *   `spriteSize`: The visual dimensions used to draw the texture.
    *   `hitboxSize`: The physical dimensions of the collision box.
    *   `hitboxOffset`: Offset positioning the hitbox relative to the sprite (centers or offsets it).
*   **Fallback Rendering**: If `textureID` is empty, renders a debug colored box using `DrawRectangleRec(getBoundingBox(), debugColor)`.
*   **Safe Margins**: Keeping hitboxes smaller than visual sprites ensures fair gameplay (prevents frustrating edge clipping deaths).

---

## 4. Collision Resolution (Axis-Separated Look-Ahead & Recovery)
*   **Predictive Look-Ahead (Primary)**:
    1.  *Horizontal*: Project `x + vx * dt`. If overlapping a solid block, set `vx = 0` and keep current X.
    2.  *Vertical*: Project `y + vy * dt`. If overlapping a solid block and falling, snap entity Y to block top, set `vy = 0`, and set `onGround = true`.
*   **Emergency Stuck Recovery (Fallback)**:
    *   If dynamic platforms or bugs cause an entity to start a frame clipping a solid, calculate overlap and push it out along the shortest axis, setting that velocity component to 0.
*   **Solid Filters**: Solid checks are only evaluated if both entities have `isSolid == true`, allowing entities like Piranha Plants inside pipes or items spawning inside blocks to bypass collision temporarily.

---

## 5. Design Patterns Blueprint

1.  **Singleton Pattern**:
    *   *Managers*: `GameEngine`, `AssetManager`, `SoundManager`.
2.  **State Pattern**:
    *   *Game States*: Managed using `std::stack<GameState*>` (MainMenu, Gameplay, Settings, Pause, GameOver).
    *   *Player States*: Player delegates updates, input, and damage to a `PlayerPowerState*` pointer (subclasses: `SmallState`, `SuperState`, `FireState`).
3.  **Factory Method Pattern**:
    *   `EntityFactory::createEntity(char type, float x, float y)` translates text characters in map arrays into concrete subclasses at calculated coordinates.
4.  **Observer Pattern**:
    *   `EventManager` maps `EventType` changes. `SoundManager`, `ScoreManager`, and `GameStateManager` subscribe to trigger audios, scores, and state transitions.
5.  **Strategy Pattern**:
    *   `AIBehaviorStrategy` decouples movement AI from concrete `Enemy` classes (e.g., Goombas patrol, RockHeads vertical drop).

---

## 6. Core Class Hierarchies & Declarations

### Systems

*   **`GameEngine`**: Coordinates window initialization and loop checks.
*   **`GameStateManager`**: Handles pushing/popping screens.
*   **`InputManager`**: Decouples keys from actions (e.g. `Action::Jump`).
*   **`Account`**: Data model for credentials, progress, and settings.
*   **`SaveManager`**: Text file stream reader/writer (`std::ifstream` and `std::ofstream`).
*   **`Camera`**: Updates coordinates relative to targets.
*   **`CollisionManager`**: Directs look-ahead sweeps and stuck recovery translations.
*   **`Level`**: Owns `std::vector<std::unique_ptr<Entity>> entities`, direct player pointer, and camera. Coordinates updates and collision sweeps.

### Entities

*   **`Entity` (Base)**: `position`, `spriteSize`, `hitboxSize`, `hitboxOffset`, `textureID`, `debugColor`, `active`, and rendering methods.
*   **`DynamicEntity`**: Adds physics kinematics: `velocity`, `acceleration`, `onGround`, `facingRight`, and collision callbacks.
*   **`Player`**: Tracks inventories (score, coins, lives), controls jumping, and delegates to power states.
*   **`Fireball`**: Projects forward, bounces on bottom solid collision, deactivates/explodes on walls or hitting enemies.
*   **`Enemy`**: Base for hostile NPCs. Holds `AIBehaviorStrategy*` pointer.
    *   `Goomba`: Flattens and dies when stomped.
    *   `Koopa`: Retreats into shell on stomp. Kicked shell slides and inflicts damage.
    *   `PiranhaPlant`: Vertical emerging behavior, immune to stomps.
    *   `RockHead`: Drops to crush player.
*   **`StaticEntity`**: Base for immovable blocks.
    *   `Block`: Solid ground.
    *   `InteractiveBlock`: Handles question blocks and brick shattering.
*   **`Item`**: Spawns from blocks (Mushroom, FireFlower, Star, Coin).
