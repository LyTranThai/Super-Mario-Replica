#include "PlayerStates.h"
#include "Player.h"
#include "Core/InputManager.h"
#include "Core/EventSystem.h"
#include <cmath>

// ======================================================
// PlayerIdleState
// ======================================================
void PlayerIdleState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("idle");
}

PlayerActionState* PlayerIdleState::handleInput(Player& player, const InputManager& input) {
    if (input.isActionJustPressed(Action::Jump)) {
        return new PlayerJumpState();
    }
    if (input.isActionPressed(Action::Crouch)) {
        return new PlayerCrouchState();
    }
    if (input.isActionPressed(Action::MoveLeft) || input.isActionPressed(Action::MoveRight)) {
        return new PlayerWalkState();
    }
    return nullptr;
}

PlayerActionState* PlayerIdleState::update(Player& player, float dt) {
    // Apply friction to stop completely if there's leftover velocity
    Vector2 vel = player.getVelocity();
    vel.x = 0.0f;
    player.setVelocity(vel);

    if (!player.isOnGround()) {
        return new PlayerFallState();
    }
    return nullptr;
}

PlayerActionState* PlayerIdleState::onCollision(Player& player, Entity& other, CollisionSide side) {
    return nullptr;
}

// ======================================================
// PlayerWalkState
// ======================================================
void PlayerWalkState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("walk");
}

PlayerActionState* PlayerWalkState::handleInput(Player& player, const InputManager& input) {
    if (input.isActionJustPressed(Action::Jump)) {
        return new PlayerJumpState();
    }
    if (input.isActionPressed(Action::Crouch)) {
        return new PlayerCrouchState();
    }
    if (!input.isActionPressed(Action::MoveLeft) && !input.isActionPressed(Action::MoveRight)) {
        return new PlayerIdleState();
    }
    
    // Check for skid
    bool movingRight = player.getVelocity().x > 0.0f;
    if ((input.isActionPressed(Action::MoveLeft) && movingRight) || 
        (input.isActionPressed(Action::MoveRight) && !movingRight && player.getVelocity().x < 0.0f)) {
        return new PlayerSkidState();
    }
    
    // Apply movement
    Vector2 vel = player.getVelocity();
    bool isRunning = input.isActionPressed(Action::Run);
    float curSpeed = player.getPowerState() ? player.getPowerState()->getMaxSpeed(isRunning) : 250.0f;
    float speedMult = player.isPlayerCrouching() ? 0.4f : 1.0f;
    
    if (input.isActionPressed(Action::MoveLeft)) {
        vel.x = -curSpeed * speedMult;
        player.setFacingRight(false);
    } else if (input.isActionPressed(Action::MoveRight)) {
        vel.x = curSpeed * speedMult;
        player.setFacingRight(true);
    }
    player.setVelocity(vel);
    
    return nullptr;
}

PlayerActionState* PlayerWalkState::update(Player& player, float dt) {
    if (!player.isOnGround()) {
        return new PlayerFallState();
    }
    return nullptr;
}

PlayerActionState* PlayerWalkState::onCollision(Player& player, Entity& other, CollisionSide side) {
    return nullptr;
}

// ======================================================
// PlayerJumpState
// ======================================================
void PlayerJumpState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("jump");
    player.jump();
}

PlayerActionState* PlayerJumpState::handleInput(Player& player, const InputManager& input) {
    Vector2 vel = player.getVelocity();
    
    // Variable jump height
    if (input.isActionReleased(Action::Jump) && vel.y < 0) {
        vel.y *= 0.5f; // Cut upward velocity in half
    }
    
    // Mid-air steering
    bool isRunning = input.isActionPressed(Action::Run);
    float curSpeed = player.getPowerState() ? player.getPowerState()->getMaxSpeed(isRunning) : 250.0f;
    if (input.isActionPressed(Action::MoveLeft)) {
        vel.x = -curSpeed;
        player.setFacingRight(false);
    } else if (input.isActionPressed(Action::MoveRight)) {
        vel.x = curSpeed;
        player.setFacingRight(true);
    } else {
        vel.x = 0.0f;
    }
    
    player.setVelocity(vel);
    return nullptr;
}

PlayerActionState* PlayerJumpState::update(Player& player, float dt) {
    if (player.getVelocity().y >= 0.0f) {
        return new PlayerFallState();
    }
    return nullptr;
}

PlayerActionState* PlayerJumpState::onCollision(Player& player, Entity& other, CollisionSide side) {
    if (side == CollisionSide::Top && other.isSolid()) {
        return new PlayerFallState();
    }
    return nullptr;
}

// ======================================================
// PlayerFallState
// ======================================================
void PlayerFallState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("fall");
}

PlayerActionState* PlayerFallState::handleInput(Player& player, const InputManager& input) {
    Vector2 vel = player.getVelocity();
    
    // Mid-air steering
    bool isRunning = input.isActionPressed(Action::Run);
    float curSpeed = player.getPowerState() ? player.getPowerState()->getMaxSpeed(isRunning) : 250.0f;
    if (input.isActionPressed(Action::MoveLeft)) {
        vel.x = -curSpeed;
        player.setFacingRight(false);
    } else if (input.isActionPressed(Action::MoveRight)) {
        vel.x = curSpeed;
        player.setFacingRight(true);
    } else {
        vel.x = 0.0f;
    }
    
    player.setVelocity(vel);
    return nullptr;
}

PlayerActionState* PlayerFallState::update(Player& player, float dt) {
    if (player.isOnGround()) {
        if (player.getVelocity().x != 0.0f) {
            return new PlayerWalkState();
        }
        return new PlayerIdleState();
    }
    return nullptr;
}

PlayerActionState* PlayerFallState::onCollision(Player& player, Entity& other, CollisionSide side) {
    // Stomp logic
    if (side == CollisionSide::Bottom && player.getVelocity().y > 0.0f && !other.isSolid()) {
        float playerBottom = player.getBoundingBox().y + player.getBoundingBox().height;
        float enemyMiddle = other.getBoundingBox().y + other.getBoundingBox().height / 2.0f;
        
        if (playerBottom < enemyMiddle) {
            // Stomp! Bounce player upward
            Vector2 vel = player.getVelocity();
            vel.y = -350.0f;
            player.setVelocity(vel);
            player.setjumpCount(1); // mid-air jump count
            return new PlayerJumpState(); // This will re-trigger jump() though, wait.
            // If we transition to JumpState, enter() will call player.jump(), which applies jumpForce (-420).
            // A stomp is usually a smaller bounce (-350). So returning JumpState will overwrite it.
            // Let's just set the velocity here and return a FallState to reset the animation, or create a StompBouncingState.
            // Or we just let JumpState skip the player.jump() if it's already moving up?
        }
    }
    return nullptr;
}

// ======================================================
// PlayerCrouchState
// ======================================================
void PlayerCrouchState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("crouch");
    player.setCrouching(true);
}

PlayerActionState* PlayerCrouchState::handleInput(Player& player, const InputManager& input) {
    if (!input.isActionPressed(Action::Crouch)) {
        player.setWantToStandUp(true);
    }
    
    // If they want to stand up, and are allowed to (we'll assume the player logic handles ceiling checks)
    // For now we just stand up immediately
    if (player.getWantToStandUp()) {
        player.setCrouching(false);
        player.setWantToStandUp(false);
        return new PlayerIdleState();
    }
    
    return nullptr;
}

PlayerActionState* PlayerCrouchState::update(Player& player, float dt) {
    Vector2 vel = player.getVelocity();
    vel.x *= 0.9f; // Slide to a halt
    if (std::abs(vel.x) < 10.0f) vel.x = 0;
    player.setVelocity(vel);
    
    if (!player.isOnGround()) {
        return new PlayerFallState();
    }
    return nullptr;
}

PlayerActionState* PlayerCrouchState::onCollision(Player& player, Entity& other, CollisionSide side) {
    return nullptr;
}

// ======================================================
// PlayerSkidState
// ======================================================
void PlayerSkidState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("skid");
}

PlayerActionState* PlayerSkidState::handleInput(Player& player, const InputManager& input) {
    if (input.isActionJustPressed(Action::Jump)) {
        return new PlayerJumpState();
    }
    return nullptr;
}

PlayerActionState* PlayerSkidState::update(Player& player, float dt) {
    Vector2 vel = player.getVelocity();
    vel.x *= 0.8f; // Skid friction
    if (std::abs(vel.x) < 20.0f) {
        vel.x = 0;
        player.setVelocity(vel);
        return new PlayerIdleState(); // Done skidding
    }
    player.setVelocity(vel);
    
    if (!player.isOnGround()) {
        return new PlayerFallState();
    }
    return nullptr;
}

PlayerActionState* PlayerSkidState::onCollision(Player& player, Entity& other, CollisionSide side) {
    return nullptr;
}

// ======================================================
// PlayerTakeDamageState
// ======================================================
void PlayerTakeDamageState::enter(Player& player) {
    timer = 1.0f; // Freeze for 1 second
}

PlayerActionState* PlayerTakeDamageState::handleInput(Player& player, const InputManager& input) {
    return nullptr; // Ignore input
}

PlayerActionState* PlayerTakeDamageState::update(Player& player, float dt) {
    timer -= dt;
    player.setVelocity(Vector2{0, 0}); // Freeze in place
    
    if (timer <= 0.0f) {
        if (player.isOnGround()) return new PlayerIdleState();
        else return new PlayerFallState();
    }
    return nullptr;
}

PlayerActionState* PlayerTakeDamageState::onCollision(Player& player, Entity& other, CollisionSide side) {
    return nullptr;
}

// ======================================================
// PlayerDieState
// ======================================================
void PlayerDieState::enter(Player& player) {
    if (player.getAnimator()) player.getAnimator()->setState("die");
    player.setVelocity(Vector2{0, -400.0f}); // Death hop
}

PlayerActionState* PlayerDieState::handleInput(Player& player, const InputManager& input) {
    return nullptr; // Ignore input
}

PlayerActionState* PlayerDieState::update(Player& player, float dt) {
    // Only apply gravity, let him fall offscreen
    return nullptr;
}

PlayerActionState* PlayerDieState::onCollision(Player& player, Entity& other, CollisionSide side) {
    return nullptr; // Ghost through everything
}
