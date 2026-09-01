#include "Player.h"
#include "Core/InputManager.h"
#include "Core/EventSystem.h"
#include "Core/AssetManager.h"
#include "Core/GameEngine.h"
#include "Koopa.h"
#include <iostream>
#include <cmath>
#include <algorithm>

struct FireballSpawnData
{
    Vector2 position;
    bool facingRight;
};

Player::Player(Vector2 pos, CharacterType type)
    : DynamicEntity(pos, Vector2{32.0f, 32.0f}, Vector2{20.0f, 26.0f}, Vector2{6.0f, 6.0f}, (type == CharacterType::Luigi ? "luigi" : "mario"), (type == CharacterType::Luigi ? GREEN : RED)),
      charType(type), lives(3), score(0), coins(0), jumpCount(0), invincibilityTimer(0.0f), isCrouching(false), wantToStandUp(false)
{

    if (charType == CharacterType::Luigi)
    {
        jumpForce = 480.0f; // Luigi jumps higher!
        speed = 230.0f;     // Slightly slower speed
    }
    else
    {
        jumpForce = 420.0f; // Mario standard jump
        speed = 250.0f;
    }

    powerState = new SmallState();
    specialMove = std::make_unique<FireballMove>();
    applyHitboxDimensions();
    configureAnimations();
}

Player::~Player()
{
    delete powerState;
}

void Player::update(float dt)
{
    if (isPipingFlag)
    {
        if (pipingTimer == 0.0f)
        {
            EventManager::getInstance().broadcast(EventType::PipeWarp);
        }
        pipingTimer += dt;
        velocity = Vector2{0.0f, 30.0f}; // Sink down slowly
        position.y += velocity.y * dt;

        animator.setState(AnimState::Pipe);
        animator.update(dt);

        if (pipingTimer >= 1.5f)
        {
            isPipingFlag = false;
            velocity = Vector2{0.0f, 0.0f};
            if (isExitPipe)
            {
                EventManager::getInstance().broadcast(EventType::LevelCompleted);
            }
            else
            {
                position = pipeTargetPos;
                position.y -= 32.0f; // Pop out
            }
        }
        return;
    }

    if (invincibilityTimer > 0.0f)
    {
        invincibilityTimer -= dt;
    }

    powerState->update(*this, dt);
    applyHitboxDimensions();
    updateAnimationState();
    animator.update(dt);

    if (carriedEntity)
    {
        float offsetX = facingRight ? hitboxSize.x + 2.0f : -carriedEntity->getHitboxSize().x - 2.0f;
        carriedEntity->setPosition(Vector2{position.x + offsetX, position.y});
    }
}

void Player::draw()
{
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id == 0 && charType == CharacterType::Luigi)
    {
        // Fallback to mario texture if luigi texture is not loaded
        tex = AssetManager::getInstance().getTexture("mario");
    }

    if (tex.id != 0)
    {
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

        Rectangle dest = {destX, destY, destWidth, destHeight};

        if (!facingRight)
        {
            source.width = -source.width; // Flip horizontally when facing left
        }

        Vector2 origin = {0.0f, 0.0f};

        Color tint = (charType == CharacterType::Luigi) ? Color{120, 255, 120, 255} : WHITE;
        if (isInvincible())
        {
            // Flashes the alpha
            if (((int)(invincibilityTimer * 15) % 2) == 0)
            {
                tint = Fade(tint, 0.2f);
            }
        }
        DrawTexturePro(tex, source, dest, origin, 0.0f, tint);
    }
    else
    {
        // Draw debug fallback box
        Color color = isInvincible() ? ORANGE : (charType == CharacterType::Luigi ? GREEN : RED);
        DrawRectangleRec(getBoundingBox(), color);
        DrawRectangleLinesEx(getBoundingBox(), 1.0f, BLACK);
    }

    // Draw the entity that it carried along
    if (carriedEntity)
    {
        // Draw it infront of player face
        float offsetX = facingRight ? hitboxSize.x + 2.0f : -carriedEntity->getHitboxSize().x - 2.0f;
        Vector2 pos = {position.x + offsetX, position.y};
        // Set location logic
        carriedEntity->setPosition(pos);
        carriedEntity->draw();
    }
}

void Player::handleInput(const InputManager &input)
{
    if (isPipingFlag)
        return;

    // Throw the carried entity if the player releases the Run button
    if (carriedEntity != nullptr && !input.isActionPressed(Action::Run))
    {
        throwCarriedEntity();
    }

    if (input.isActionJustPressed(Action::Crouch))
    {
        if (isCrouching)
        {
            wantToStandUp = true;
        }
        else
        {
            isCrouching = true;
            wantToStandUp = false;
        }
    }
    if (input.isActionJustPressed(Action::Jump))
    {
        if (isCrouching)
        {
            wantToStandUp = true;
        }
    }
    if (input.isActionJustPressed(Action::Shoot))
    {
        if (carriedEntity != nullptr)
        {
            throwCarriedEntity();
        }
        else if (specialMove)
        {
            specialMove->execute(*this);
        }
    }

    powerState->handleInput(*this, input);
}

void Player::jump()
{
    if (onGround)
    {
        velocity.y = -jumpForce;
        onGround = false;
        jumpCount = 1;
        EventManager::getInstance().broadcast(EventType::PlayerJump);
    }
    else if (jumpCount == 0)
    {
        // Mid-air jump after falling/bouncing off enemy
        velocity.y = -jumpForce;
        jumpCount = 1;
        EventManager::getInstance().broadcast(EventType::PlayerJump);
    }
    else if (jumpCount == 1)
    {
        // Double jump!
        velocity.y = -jumpForce;
        jumpCount = 2;
        EventManager::getInstance().broadcast(EventType::PlayerJump);
    }
}

void Player::takeDamage()
{
    if (isInvincible())
        return;

    std::cout << "[DEBUG] Player (" << textureID << ") took damage! PowerState: "
              << (getPowerType() == PowerStateType::Small ? "Small" : (getPowerType() == PowerStateType::Super ? "Super" : "Fire"))
              << ", Lives remaining: " << lives << std::endl;

    powerState->onDamage(*this);
    invincibilityTimer = 2.0f; // 2 seconds of recovery invincibility
}

void Player::powerUp(PowerStateType type)
{
    if (type == PowerStateType::Super && getPowerType() == PowerStateType::Small)
    {
        changePowerState(new SuperState());
        addScore(1000);
    }
    else if (type == PowerStateType::Fire)
    {
        changePowerState(new FireState());
        addScore(1000);
    }
}

void Player::changePowerState(PlayerPowerState *newState)
{
    delete powerState;
    powerState = newState;

    if (powerState->getType() == PowerStateType::Fire)
    {
        setSpecialMove(std::make_unique<FireballMove>());
    }
    else
    {
        setSpecialMove(std::make_unique<NoneMove>());
    }

    // Resize hitbox and graphics boundaries dynamically
    Vector2 oldSize = hitboxSize;
    Vector2 oldOffset = hitboxOffset;
    applyHitboxDimensions();
    configureAnimations(); // Reconfigure animation frames for the new power state

    // Adjust position vertically to prevent clipping into the ground when growing or floating when shrinking
    float oldBottomRel = oldOffset.y + oldSize.y;
    float newBottomRel = hitboxOffset.y + hitboxSize.y;
    position.y -= (newBottomRel - oldBottomRel);
}

void Player::setSpecialMove(std::unique_ptr<SpecialMove> move)
{
    specialMove = std::move(move);
}

void Player::throwCarriedEntity()
{
    if (carriedEntity)
    {
        bool drop = GameEngine::getInstance().getInputManager().isActionPressed(Action::Crouch);

        if (!drop && dynamic_cast<Koopa *>(carriedEntity))
        {
            EventManager::getInstance().broadcast(EventType::KoopaKicked);
        }

        carriedEntity->setPosition(Vector2{position.x + (facingRight ? hitboxSize.x + 5.0f : -carriedEntity->getHitboxSize().x - 5.0f), position.y});

        if (drop)
        {
            carriedEntity->setVelocity(Vector2{0.0f, 0.0f});
        }
        else
        {
            carriedEntity->setVelocity(Vector2{facingRight ? 400.0f : -400.0f, -100.0f});
        }

        // Base throwing logic; specific entities can check if they are carried inside their own updates

        carriedEntity = nullptr;
    }
}

void Player::shootFireball()
{
    std::cout << "[DEBUG]" << textureID << " shoot fireball" << std::endl;
    Vector2 spawnPos;
    if (facingRight)
    {
        spawnPos.x = position.x + spriteSize.x;
    }
    else
    {
        spawnPos.x = position.x - 16.0f;
    }
    spawnPos.y = position.y + spriteSize.y / 2.0f - 8.0f;

    FireballSpawnData data = {spawnPos, facingRight};
    EventManager::getInstance().broadcast(EventType::FireballShot, &data);
}

void Player::onCollision(Entity &other, CollisionSide side)
{
    if (side == CollisionSide::Bottom)
    {
        jumpCount = 0;
    }

    // Stomp logic
    DynamicEntity *dynOther = dynamic_cast<DynamicEntity *>(&other);
    if (dynOther && !other.isSolid())
    {
        float playerBottom = getBoundingBox().y + getBoundingBox().height;
        float enemyMiddle = other.getBoundingBox().y + other.getBoundingBox().height / 2.0f;

        if (playerBottom < enemyMiddle && velocity.y > 0.0f)
        {
            // Stomp! Bounce player upward
            velocity.y = -350.0f;
        }
    }
}

void Player::setOnGround(bool state)
{
    DynamicEntity::setOnGround(state);
    if (state)
    {
        jumpCount = 0;
    }
}

void Player::getPowerStateDimensions(Vector2 &outSpriteSize, Vector2 &outHitboxSize, Vector2 &outHitboxOffset) const
{
    if (powerState)
    {
        powerState->getDimensions(outSpriteSize, outHitboxSize, outHitboxOffset);
    }
}

void Player::applyHitboxDimensions()
{
    Vector2 baseSpriteSize, baseHitboxSize, baseHitboxOffset;
    getPowerStateDimensions(baseSpriteSize, baseHitboxSize, baseHitboxOffset);

    if (isCrouching)
    {
        spriteSize = baseSpriteSize;
        spriteSize.y = baseSpriteSize.y * crouchHeightPercentage;

        hitboxSize = baseHitboxSize;
        hitboxSize.y = baseHitboxSize.y * crouchHeightPercentage;

        hitboxOffset = baseHitboxOffset;
        hitboxOffset.y = baseHitboxOffset.y + (baseHitboxSize.y - hitboxSize.y);
    }
    else
    {
        spriteSize = baseSpriteSize;
        hitboxSize = baseHitboxSize;
        hitboxOffset = baseHitboxOffset;
    }
}

Rectangle Player::getSpriteBox() const
{
    Vector2 baseSpriteSize, baseHitboxSize, baseHitboxOffset;
    getPowerStateDimensions(baseSpriteSize, baseHitboxSize, baseHitboxOffset);
    float offsetY = baseSpriteSize.y - spriteSize.y;
    return Rectangle{position.x, position.y + offsetY, spriteSize.x, spriteSize.y};
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

void Player::configureAnimations()
{
    animator.clearAnimations();

    PowerStateType type = powerState->getType();

    if (type == PowerStateType::Small) {
        if (charType == CharacterType::Luigi) {
            animator.addAnimation(AnimState::Idle, { Rectangle{ 215.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Walk, { Rectangle{ 239.0f, 24.0f, 16.0f, 16.0f }, Rectangle{ 256.0f, 24.0f, 16.0f, 16.0f } }, 0.1f);
            animator.addAnimation(AnimState::Jump, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Fall, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Skid, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Die, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f, false);
            animator.addAnimation(AnimState::Crouch, { Rectangle{ 235.0f, 53.0f, 13.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Pipe, { Rectangle{ 284.0f, 53.0f, 16.0f, 16.0f } }, 1.0f);
        } else {
            animator.addAnimation(AnimState::Idle, { Rectangle{ 2.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Walk, { Rectangle{ 26.0f, 24.0f, 16.0f, 16.0f }, Rectangle{ 43.0f, 24.0f, 16.0f, 16.0f } }, 0.1f);
            animator.addAnimation(AnimState::Jump, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Fall, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Skid, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Die, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f, false);
            animator.addAnimation(AnimState::Crouch, { Rectangle{ 19.0f, 53.0f, 16.0f, 16.0f } }, 1.0f);
            animator.addAnimation(AnimState::Pipe, { Rectangle{ 71.0f, 53.0f, 16.0f, 16.0f } }, 1.0f);
        }
    } else if (type == PowerStateType::Super) {
        if (charType == CharacterType::Luigi) {
            animator.addAnimation(AnimState::Idle, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Walk, { Rectangle{ 289.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 322.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
            animator.addAnimation(AnimState::Jump, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Fall, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Skid, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Die, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
            animator.addAnimation(AnimState::Crouch, { Rectangle{ 252.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Pipe, { Rectangle{ 215.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
        } else {
            animator.addAnimation(AnimState::Idle, { Rectangle{ 2.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Walk, { Rectangle{ 76.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 109.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
            animator.addAnimation(AnimState::Jump, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Fall, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Skid, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Die, { Rectangle{ 2.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
            animator.addAnimation(AnimState::Crouch, { Rectangle{ 33.0f, 88.0f, 38.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Pipe, { Rectangle{ 2.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
        }
    } else if (type == PowerStateType::Fire) {
        if (charType == CharacterType::Luigi) {
            animator.addAnimation(AnimState::Idle, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Walk, { Rectangle{ 289.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 322.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
            animator.addAnimation(AnimState::Jump, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Fall, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Skid, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Die, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
            animator.addAnimation(AnimState::Crouch, { Rectangle{ 252.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Pipe, { Rectangle{ 215.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
        } else {
            animator.addAnimation(AnimState::Idle, { Rectangle{ 2.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Walk, { Rectangle{ 76.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 109.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
            animator.addAnimation(AnimState::Jump, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Fall, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Skid, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Die, { Rectangle{ 2.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
            animator.addAnimation(AnimState::Crouch, { Rectangle{ 33.0f, 88.0f, 38.0f, 32.0f } }, 1.0f);
            animator.addAnimation(AnimState::Pipe, { Rectangle{ 2.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
        }
    }
}

void Player::updateAnimationState()
{
    if (!onGround)
    {
        if (velocity.y < 0.0f)
        {
            animator.setState(AnimState::Jump);
        }
        else
        {
            animator.setState(AnimState::Fall);
        }
    }
    else if (isCrouching)
    {
        animator.setState(AnimState::Crouch);
    }
    else if (velocity.x != 0.0f)
    {
        // Check for skid: moving one direction but facing the other
        bool movingRight = velocity.x > 0.0f;
        if (movingRight != facingRight)
        {
            animator.setState(AnimState::Skid);
        }
        else
        {
            animator.setState(AnimState::Walk);
        }
    }
    else
    {
        animator.setState(AnimState::Idle);
    }
}
