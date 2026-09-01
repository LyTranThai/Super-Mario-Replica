#include "Mario.h"
#include "Core/AssetManager.h"

Mario::Mario(Vector2 pos) : FireballPlayer(pos, "mario", RED) {
    jumpForce = 420.0f;
    speed = 250.0f;
    configureAnimations();
}

void Mario::configureAnimations() {
    animator.clearAnimations();
    PowerStateType type = getPowerType();

    if (type == PowerStateType::Small) {
        animator.addAnimation(AnimState::Idle, { Rectangle{ 2.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Walk, { Rectangle{ 26.0f, 24.0f, 16.0f, 16.0f }, Rectangle{ 43.0f, 24.0f, 16.0f, 16.0f } }, 0.1f);
        animator.addAnimation(AnimState::Jump, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Fall, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Skid, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Die, { Rectangle{ 109.0f, 24.0f, 16.0f, 16.0f } }, 1.0f, false);
        animator.addAnimation(AnimState::Crouch, { Rectangle{ 19.0f, 53.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Pipe, { Rectangle{ 71.0f, 53.0f, 16.0f, 16.0f } }, 1.0f);
    } else if (type == PowerStateType::Super) {
        animator.addAnimation(AnimState::Idle, { Rectangle{ 2.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Walk, { Rectangle{ 76.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 109.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
        animator.addAnimation(AnimState::Jump, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Fall, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Skid, { Rectangle{ 2.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Die, { Rectangle{ 2.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
        animator.addAnimation(AnimState::Crouch, { Rectangle{ 33.0f, 88.0f, 38.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Pipe, { Rectangle{ 2.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
    } else if (type == PowerStateType::Fire) {
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
