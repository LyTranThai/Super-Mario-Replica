#include "Luigi.h"
#include "Core/AssetManager.h"

Luigi::Luigi(Vector2 pos) : FireballPlayer(pos, "luigi", GREEN) {
    jumpForce = 440.0f; // Slightly higher than Mario (420.0f)
    speed = 230.0f;     // Slightly slower speed
    configureAnimations();
}

void Luigi::configureAnimations() {
    animator.clearAnimations();
    PowerStateType type = getPowerType();

    if (type == PowerStateType::Small) {
        animator.addAnimation(AnimState::Idle, { Rectangle{ 215.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Walk, { Rectangle{ 239.0f, 24.0f, 16.0f, 16.0f }, Rectangle{ 256.0f, 24.0f, 16.0f, 16.0f } }, 0.1f);
        animator.addAnimation(AnimState::Jump, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Fall, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Skid, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Die, { Rectangle{ 322.0f, 24.0f, 16.0f, 16.0f } }, 1.0f, false);
        animator.addAnimation(AnimState::Crouch, { Rectangle{ 235.0f, 53.0f, 13.0f, 16.0f } }, 1.0f);
        animator.addAnimation(AnimState::Pipe, { Rectangle{ 284.0f, 53.0f, 16.0f, 16.0f } }, 1.0f);
    } else if (type == PowerStateType::Super) {
        animator.addAnimation(AnimState::Idle, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Walk, { Rectangle{ 289.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 322.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
        animator.addAnimation(AnimState::Jump, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Fall, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Skid, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Die, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
        animator.addAnimation(AnimState::Crouch, { Rectangle{ 252.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Pipe, { Rectangle{ 215.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
    } else if (type == PowerStateType::Fire) {
        animator.addAnimation(AnimState::Idle, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Walk, { Rectangle{ 289.0f, 88.0f, 32.0f, 32.0f }, Rectangle{ 322.0f, 88.0f, 32.0f, 32.0f } }, 0.1f);
        animator.addAnimation(AnimState::Jump, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Fall, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Skid, { Rectangle{ 215.0f, 132.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Die, { Rectangle{ 215.0f, 88.0f, 32.0f, 32.0f } }, 1.0f, false);
        animator.addAnimation(AnimState::Crouch, { Rectangle{ 252.0f, 88.0f, 32.0f, 32.0f } }, 1.0f);
        animator.addAnimation(AnimState::Pipe, { Rectangle{ 215.0f, 176.0f, 32.0f, 32.0f } }, 1.0f);
    }
}

Color Luigi::getSpriteTint() const {
    return Color{120, 255, 120, 255};
}

Texture2D Luigi::getSpriteTexture() const {
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id == 0) {
        return AssetManager::getInstance().getTexture("mario");
    }
    return tex;
}
