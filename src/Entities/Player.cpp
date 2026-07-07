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
    powerState = new FireState();
    specialMove = std::make_unique<NoneMove>();
    applyHitboxDimensions();
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
    
    if (carriedEntity) {
        float offsetX = facingRight ? hitboxSize.x + 2.0f : -carriedEntity->getHitboxSize().x - 2.0f;
        carriedEntity->setPosition(Vector2{ position.x + offsetX, position.y });
    }
}

void Player::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0) {
        Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        if (!facingRight) {
            source.width = -source.width;
        }
        
        Rectangle dest = getSpriteBox();
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

    //Draw the entity that it carried along
    if(carriedEntity)
    {
        //Draw it infront of player face
        float offsetX = facingRight ? hitboxSize.x + 2.0f : -carriedEntity->getHitboxSize().x - 2.0f;
        Vector2 pos = { position.x + offsetX, position.y };
        // Set location logic
        carriedEntity -> setPosition(pos);
        carriedEntity -> draw();
    }
}

void Player::handleInput(const InputManager& input) {
    // Drop the carried entity if the player releases the Run button
    if (carriedEntity != nullptr && !input.isActionPressed(Action::Run)) {
        if (Koopa* koopa = dynamic_cast<Koopa*>(carriedEntity)) {
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

    std::cout << "[DEBUG] Player (" << textureID << ") took damage! PowerState: " 
              << (getPowerType() == PowerStateType::Small ? "Small" : (getPowerType() == PowerStateType::Super ? "Super" : "Fire")) 
              << ", Lives remaining: " << lives << std::endl;

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
     
    //std::cout << "[DEBUG] Collision: Player (" << textureID << ") collided with (" << other.getTextureID() << ") on side " 
    //          << (side == CollisionSide::Top ? "Top" : (side == CollisionSide::Bottom ? "Bottom" : (side == CollisionSide::Left ? "Left" : (side == CollisionSide::Right ? "Right" : "None")))) << std::endl;

    if (side == CollisionSide::Bottom) {
        //std::cout << "[DEBUG]   -> Case: Player hit ground/solid bottom. Resetting jumpCount." << std::endl;
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
