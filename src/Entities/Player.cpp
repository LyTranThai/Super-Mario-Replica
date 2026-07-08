#include "Player.h"
#include "Core/InputManager.h"
#include "Core/EventSystem.h"
#include "Core/AssetManager.h"
#include "Koopa.h"
#include <iostream>

struct FireballSpawnData {
    Vector2 position;
    bool facingRight;
};

Player::Player(Vector2 pos) 
    : DynamicEntity(pos, Vector2{ 32.0f, 32.0f }, Vector2{ 20.0f, 26.0f }, Vector2{ 6.0f, 6.0f }, "mario", RED),
      lives(3), score(0), coins(0), jumpCount(0), invincibilityTimer(0.0f), isCrouching(false), wantToStandUp(false) {
    
    powerState = new SmallState();
    specialMove = std::make_unique<NoneMove>();
    applyHitboxDimensions();
    configureAnimations();
}

Player::~Player() {
    delete powerState;
}

void Player::update(float dt) {
    if (invincibilityTimer > 0.0f) {
        invincibilityTimer -= dt;
    }

    powerState->update(*this, dt);
    applyHitboxDimensions();
    updateAnimationState();
    animator.update(dt);
    
    if (carriedEntity) {
        float offsetX = facingRight ? hitboxSize.x + 2.0f : -carriedEntity->getHitboxSize().x - 2.0f;
        carriedEntity->setPosition(Vector2{ position.x + offsetX, position.y });
    }
}

void Player::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        // Get the current animation frame from the spritesheet
        Rectangle source = animator.getCurrentFrame();
        float srcWidth = std::abs(source.width);
        float srcHeight = std::abs(source.height);
        
        // Calculate scale to match player's current sprite height
        float scale = spriteSize.y / srcHeight;
        float destWidth = srcWidth * scale;
        float destHeight = spriteSize.y;
        
        // Center the sprite horizontally on the player's hitbox
        float hitboxCenter = position.x + hitboxOffset.x + hitboxSize.x / 2.0f;
        float destX = hitboxCenter - destWidth / 2.0f;
        
        // Align the bottom of the sprite box
        Vector2 baseSpriteSize, baseHitboxSize, baseHitboxOffset;
        getPowerStateDimensions(baseSpriteSize, baseHitboxSize, baseHitboxOffset);
        float offsetY = baseSpriteSize.y - spriteSize.y;
        float destY = position.y + offsetY;
        
        Rectangle dest = { destX, destY, destWidth, destHeight };

        if (facingRight) {
            source.width = -source.width;  // Flip horizontally because sheet faces left
        }
        
        Vector2 origin = { 0.0f, 0.0f };

        Color tint = WHITE;
        if (isInvincible()) {
            // Flashes the alpha
            if (((int)(invincibilityTimer * 15) % 2) == 0) {
                tint = Fade(WHITE, 0.2f);
            }
        }
        DrawTexturePro(tex, source, dest, origin, 0.0f, tint);
    } else {
        // Draw debug fallback box
        Color color = isInvincible() ? ORANGE : RED;
        DrawRectangleRec(getBoundingBox(), color);
        DrawRectangleLinesEx(getBoundingBox(), 1.0f, BLACK);
    }
}

void Player::handleInput(const InputManager& input) {
    // Drop the carried entity if the player releases the Run button
    if (carriedEntity != nullptr && !input.isActionPressed(Action::Run)) {
        Koopa* koopa = dynamic_cast<Koopa*>(carriedEntity);
        if (koopa) {
            koopa->setCarried(false);
        }
        carriedEntity = nullptr;
    }

    if (input.isActionJustPressed(Action::Crouch)) {
        if (isCrouching) {
            wantToStandUp = true;
        } else {
            isCrouching = true;
            wantToStandUp = false;
        }
    }
    if (input.isActionJustPressed(Action::Jump)) {
        if (isCrouching) {
            wantToStandUp = true;
        }
    }
    if (input.isActionJustPressed(Action::Shoot)) {
        if (carriedEntity != nullptr) {
            throwCarriedEntity();
        } else if (specialMove) {
            specialMove->execute(*this);
        }
    }

    powerState->handleInput(*this, input);
}

void Player::jump() {
    if (onGround) {
        velocity.y = -jumpForce;
        onGround = false;
        jumpCount = 1;
        EventManager::getInstance().broadcast(EventType::PlayerJump);
    } else if (jumpCount == 0) {
        // Mid-air jump after falling/bouncing off enemy
        velocity.y = -jumpForce;
        jumpCount = 1;
        EventManager::getInstance().broadcast(EventType::PlayerJump);
    } else if (jumpCount == 1) {
        // Double jump!
        velocity.y = -jumpForce;
        jumpCount = 2;
        EventManager::getInstance().broadcast(EventType::PlayerJump);
    }
}

void Player::takeDamage() {
    if (isInvincible()) return;

    powerState->onDamage(*this);
    invincibilityTimer = 2.0f; // 2 seconds of recovery invincibility
}

void Player::powerUp(PowerStateType type) {
    if (type == PowerStateType::Super && getPowerType() == PowerStateType::Small) {
        changePowerState(new SuperState());
        addScore(1000);
    } else if (type == PowerStateType::Fire) {
        changePowerState(new FireState());
        addScore(1000);
    }
}

void Player::changePowerState(PlayerPowerState* newState) {
    delete powerState;
    powerState = newState;
    
    if (powerState->getType() == PowerStateType::Fire) {
        setSpecialMove(std::make_unique<FireballMove>());
    } else {
        setSpecialMove(std::make_unique<NoneMove>());
    }
    
    // Resize hitbox and graphics boundaries dynamically
    Vector2 oldSize = hitboxSize;
    applyHitboxDimensions();
    configureAnimations();  // Reconfigure animation frames for the new power state

    // Adjust position vertically to prevent clipping into the ground when growing
    position.y -= (hitboxSize.y - oldSize.y);
}

void Player::setSpecialMove(std::unique_ptr<SpecialMove> move) {
    specialMove = std::move(move);
}

void Player::throwCarriedEntity() {
    if (carriedEntity) {
        carriedEntity->setPosition(Vector2{ position.x + (facingRight ? hitboxSize.x + 5.0f : -carriedEntity->getHitboxSize().x - 5.0f), position.y });
        carriedEntity->setVelocity(Vector2{ facingRight ? 400.0f : -400.0f, -100.0f });
        
        Koopa* koopa = dynamic_cast<Koopa*>(carriedEntity);
        if (koopa) {
            koopa->setCarried(false);
            koopa->setShellMoving(true);
        }
        
        carriedEntity = nullptr;
    }
}

void Player::shootFireball() {
    Vector2 spawnPos;
    if (facingRight) {
        spawnPos.x = position.x + spriteSize.x;
    } else {
        spawnPos.x = position.x - 16.0f;
    }
    spawnPos.y = position.y + spriteSize.y / 2.0f - 8.0f;

    FireballSpawnData data = { spawnPos, facingRight };
    EventManager::getInstance().broadcast(EventType::FireballShot, &data);
}

void Player::onCollision(Entity& other, CollisionSide side) {
    (void)other;
    if (side == CollisionSide::Bottom) {
        jumpCount = 0;
    }
}

void Player::setOnGround(bool state) {
    DynamicEntity::setOnGround(state);
    if (state) {
        jumpCount = 0;
    }
}

void Player::getPowerStateDimensions(Vector2& outSpriteSize, Vector2& outHitboxSize, Vector2& outHitboxOffset) const {
    if (powerState) {
        powerState->getDimensions(outSpriteSize, outHitboxSize, outHitboxOffset);
    }
}

void Player::applyHitboxDimensions() {
    Vector2 baseSpriteSize, baseHitboxSize, baseHitboxOffset;
    getPowerStateDimensions(baseSpriteSize, baseHitboxSize, baseHitboxOffset);
    
    if (isCrouching) {
        spriteSize = baseSpriteSize;
        spriteSize.y = baseSpriteSize.y * crouchHeightPercentage;
        
        hitboxSize = baseHitboxSize;
        hitboxSize.y = baseHitboxSize.y * crouchHeightPercentage;
        
        hitboxOffset = baseHitboxOffset;
        hitboxOffset.y = baseHitboxOffset.y + (baseHitboxSize.y - hitboxSize.y);
    } else {
        spriteSize = baseSpriteSize;
        hitboxSize = baseHitboxSize;
        hitboxOffset = baseHitboxOffset;
    }
}

Rectangle Player::getSpriteBox() const {
    Vector2 baseSpriteSize, baseHitboxSize, baseHitboxOffset;
    getPowerStateDimensions(baseSpriteSize, baseHitboxSize, baseHitboxOffset);
    float offsetY = baseSpriteSize.y - spriteSize.y;
    return Rectangle{ position.x, position.y + offsetY, spriteSize.x, spriteSize.y };
}

// ============================================================
// Spritesheet Animation Configuration
// ============================================================
// Frame rectangles are derived from pixel-scanning Mario.png (442x339).
// Each Rectangle is {x, y, width, height} on the spritesheet.
//
// Spritesheet row map:
//   Row 0 (y=0,  h=16):  Small Mario poses (8 sprites)
//   Row 1 (y=39, h=17):  Small Mario poses (9 sprites)
//   Row 2 (y=68, h=40):  Super Mario poses (11 sprites)
//   Row 3 (y=112,h=31):  Super Mario poses (13 sprites)
//   Row 4 (y=148,h=40):  Fire Mario poses  (11 sprites)
//   Row 5 (y=192,h=31):  Fire Mario poses  (15 sprites)
//   Row 6-8: Swimming, climbing, misc
// ============================================================

void Player::configureAnimations() {
    animator.clearAnimations();

    PowerStateType type = powerState->getType();

    if (type == PowerStateType::Small) {
        // --- Small Luigi/Mario frames from Row 0 (y=0, h=16) and Row 1 (y=39, h=17) ---
        // Idle (standing) â€” index 0 (x=5)
        animator.addAnimation(AnimState::Idle,
            { Rectangle{5, 0, 11, 16} }, 1.0f);

        // Walk cycle â€” 3 frames (index 1, 2, 3)
        animator.addAnimation(AnimState::Walk,
            { Rectangle{34, 0, 14, 16},
              Rectangle{93, 0, 16, 16},
              Rectangle{154, 0, 13, 16} }, 0.08f);

        // Jump â€” index 5 (x=213)
        animator.addAnimation(AnimState::Jump,
            { Rectangle{213, 0, 15, 16} }, 1.0f);

        // Fall â€” same as jump
        animator.addAnimation(AnimState::Fall,
            { Rectangle{213, 0, 15, 16} }, 1.0f);

        // Skid/turn â€” index 4 (x=183)
        animator.addAnimation(AnimState::Skid,
            { Rectangle{183, 0, 16, 16} }, 1.0f);

        // Die â€” Row 1, index 8 (x=423)
        animator.addAnimation(AnimState::Die,
            { Rectangle{423, 39, 16, 17} }, 1.0f, false);

        // Crouch â€” index 6 (x=244)
        animator.addAnimation(AnimState::Crouch,
            { Rectangle{244, 0, 14, 16} }, 1.0f);

    } else if (type == PowerStateType::Super) {
        // --- Super Luigi/Mario frames from Row 2 (y=68, h=40) and Row 3 (y=112, h=31) ---
        // Idle â€” Row 2, index 0 (x=4)
        animator.addAnimation(AnimState::Idle,
            { Rectangle{4, 68, 14, 40} }, 1.0f);

        // Walk cycle â€” 3 frames (Row 2, index 1, 2, 3)
        animator.addAnimation(AnimState::Walk,
            { Rectangle{33, 68, 16, 40},
              Rectangle{63, 68, 16, 40},
              Rectangle{93, 68, 16, 40} }, 0.08f);

        // Jump â€” Row 2, index 5 (x=153)
        animator.addAnimation(AnimState::Jump,
            { Rectangle{153, 68, 16, 40} }, 1.0f);

        // Fall â€” same as jump
        animator.addAnimation(AnimState::Fall,
            { Rectangle{153, 68, 16, 40} }, 1.0f);

        // Skid â€” Row 2, index 4 (x=124)
        animator.addAnimation(AnimState::Skid,
            { Rectangle{124, 68, 14, 40} }, 1.0f);

        // Crouch â€” Row 3, index 0 (x=2, h=31)
        animator.addAnimation(AnimState::Crouch,
            { Rectangle{2, 112, 18, 31} }, 1.0f);

        // Die â€” Row 1, index 8 (x=423)
        animator.addAnimation(AnimState::Die,
            { Rectangle{423, 39, 16, 17} }, 1.0f, false);

    } else if (type == PowerStateType::Fire) {
        // --- Fire Luigi/Mario frames from Row 4 (y=148, h=40) and Row 5 (y=192, h=31) ---
        // Idle
        animator.addAnimation(AnimState::Idle,
            { Rectangle{4, 148, 14, 40} }, 1.0f);

        // Walk cycle â€” 3 frames
        animator.addAnimation(AnimState::Walk,
            { Rectangle{33, 148, 16, 40},
              Rectangle{63, 148, 16, 40},
              Rectangle{93, 148, 16, 40} }, 0.08f);

        // Jump
        animator.addAnimation(AnimState::Jump,
            { Rectangle{153, 148, 16, 40} }, 1.0f);

        // Fall
        animator.addAnimation(AnimState::Fall,
            { Rectangle{153, 148, 16, 40} }, 1.0f);

        // Skid
        animator.addAnimation(AnimState::Skid,
            { Rectangle{124, 148, 14, 40} }, 1.0f);

        // Crouch
        animator.addAnimation(AnimState::Crouch,
            { Rectangle{2, 192, 18, 31} }, 1.0f);

        // Die
        animator.addAnimation(AnimState::Die,
            { Rectangle{423, 39, 16, 17} }, 1.0f, false);
    }
}

void Player::updateAnimationState() {
    if (!onGround) {
        if (velocity.y < 0.0f) {
            animator.setState(AnimState::Jump);
        } else {
            animator.setState(AnimState::Fall);
        }
    } else if (isCrouching) {
        animator.setState(AnimState::Crouch);
    } else if (velocity.x != 0.0f) {
        // Check for skid: moving one direction but facing the other
        bool movingRight = velocity.x > 0.0f;
        if (movingRight != facingRight) {
            animator.setState(AnimState::Skid);
        } else {
            animator.setState(AnimState::Walk);
        }
    } else {
        animator.setState(AnimState::Idle);
    }
}
