#include "Camera.h"

GameCamera::GameCamera() : position{0.0f, 0.0f}, viewportSize{800.0f, 600.0f}, leftBoundary(0.0f), rightBoundary(2000.0f), isLeftLocked(false) {}

GameCamera::GameCamera(Vector2 size, float leftBound, float rightBound) 
    : position{0.0f, 0.0f}, viewportSize(size), leftBoundary(leftBound), rightBoundary(rightBound), isLeftLocked(false) {}

void GameCamera::update(Vector2 targetPos, float dt) {
    // Focus horizontally on target
    float targetX = targetPos.x - viewportSize.x / 2.0f;

    // Smooth lerp for camera
    float lerpSpeed = 8.0f; // Adjust this value to make it more or less rigid
    float nextPos = position.x + (targetX - position.x) * lerpSpeed * dt;

    // Apply left lock if enabled (never allow camera to move left)
    if (isLeftLocked && nextPos < position.x) {
        nextPos = position.x;
    }

    position.x = nextPos;

    // Clamp within level boundaries
    if (position.x < leftBoundary) {
        position.x = leftBoundary;
    }
    float maxScrollX = rightBoundary - viewportSize.x;
    if (position.x > maxScrollX) {
        position.x = maxScrollX;
    }

    // Keep Y locked at 0 (flat scroller)
    position.y = 0.0f;
}

Vector2 GameCamera::applyOffset(Vector2 worldPos) const {
    return Vector2{ worldPos.x - position.x, worldPos.y - position.y };
}

Rectangle GameCamera::applyOffsetToRect(Rectangle worldRect) const {
    return Rectangle{ worldRect.x - position.x, worldRect.y - position.y, worldRect.width, worldRect.height };
}

void GameCamera::setBoundaries(float leftBound, float rightBound) {
    leftBoundary = leftBound;
    rightBoundary = rightBound;
}

void GameCamera::setLeftLocked(bool locked) {
    isLeftLocked = locked;
}
