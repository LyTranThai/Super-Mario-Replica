#include "Fireball.h"
#include "Enemy.h"
#include "Player.h"
#include "Core/AssetManager.h"
#include <iostream>

Fireball::Fireball(Vector2 pos, bool faceRight)
    : DynamicEntity(
          pos,
          Vector2{ 16.0f, 16.0f },
          Vector2{ 14.0f, 14.0f },
          Vector2{ 1.0f, 1.0f },
          "mario",
          ORANGE),
      bounceForce(250.0f),
      speed(350.0f),
      bouncesLeft(5),
      animTimer(0.0f),
      currentFrame(0) {
    facingRight = faceRight;
    velocity.x = facingRight ? speed : -speed;
    velocity.y = 0.0f;
    onGround = false;
}

Rectangle Fireball::getCurrentFrame() const {
    // Alternating frames from player sprite sheet: (152,183)->(159,190), (162,183)->(169,190)
    if (currentFrame == 0) {
        return Rectangle{ 152.0f, 183.0f, 8.0f, 8.0f };
    } else {
        return Rectangle{ 162.0f, 183.0f, 8.0f, 8.0f };
    }
}

void Fireball::update(float dt) {
    // Keep horizontal speed
    velocity.x = facingRight ? speed : -speed;

    animTimer += dt;
    if (animTimer >= 0.08f) {
        animTimer = 0.0f;
        currentFrame = (currentFrame + 1) % 2;
    }

    if (position.y > 2000.0f || position.x < -200.0f) {
        explode();
    }
}

void Fireball::draw() {
    Texture2D tex = AssetManager::getInstance().getTexture("mario");
    if (tex.id == 0) {
        tex = AssetManager::getInstance().getTexture("luigi");
    }

    if (tex.id != 0) {
        Rectangle src = getCurrentFrame();
        if (!facingRight) {
            src.width = -src.width;
        }
        Rectangle dest = getSpriteBox();
        DrawTexturePro(tex, src, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DynamicEntity::draw();
    }
}

void Fireball::explode() {
    active = false;
}

void Fireball::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive() || !active) return;

    if (dynamic_cast<Player*>(&other)) {
        return; // Don't collide with player
    }

    Enemy* enemy = dynamic_cast<Enemy*>(&other);
    if (enemy) {
        if (enemy->isActive()) {
            enemy->takeDamage();
        }
        explode();
        return;
    }

    if (other.isSolid()) {
        if (side == CollisionSide::Bottom) {
            // Bounce upward
            velocity.y = -bounceForce;
            onGround = false;
            bouncesLeft--;
            if (bouncesLeft <= 0) {
                explode();
            }
        } else if (side == CollisionSide::Left || side == CollisionSide::Right || side == CollisionSide::Top) {
            explode();
        }
    }
}

