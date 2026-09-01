#include "Fireball.h"
#include "Enemy.h"
#include "Player.h"
#include "Core/AssetManager.h"
#include <iostream>

Fireball::Fireball(Vector2 pos, bool faceRight, FireballType type)
    : DynamicEntity(
          pos,
          (type == FireballType::Fireball2 ? Vector2{ 32.0f, 32.0f } : Vector2{ 16.0f, 16.0f }),
          (type == FireballType::Fireball2 ? Vector2{ 28.0f, 28.0f } : Vector2{ 14.0f, 14.0f }),
          (type == FireballType::Fireball2 ? Vector2{ 2.0f, 2.0f } : Vector2{ 1.0f, 1.0f }),
          "mario",
          ORANGE),
      fireballType(type),
      bounceForce(type == FireballType::Fireball2 ? 220.0f : 250.0f),
      speed(type == FireballType::Fireball2 ? 300.0f : 350.0f),
      bouncesLeft(5),
      animTimer(0.0f),
      currentFrame(0),
      stateTimer(0.0f),
      state(1) {
    facingRight = faceRight;
    velocity.x = facingRight ? speed : -speed;
    velocity.y = 0.0f;
    onGround = false;
}

Rectangle Fireball::getCurrentFrame() const {
    if (fireballType == FireballType::Fireball1) {
        // fireball 1: (152,183) -> (159,190), (162,183) -> (169,190) alternating
        if (currentFrame == 0) {
            return Rectangle{ 152.0f, 183.0f, 8.0f, 8.0f };
        } else {
            return Rectangle{ 162.0f, 183.0f, 8.0f, 8.0f };
        }
    } else {
        // fireball 2:
        // state 1: (146,192) -> (161,207)
        // state 2: (163,192) -> (178,207)
        // state 3: (180,192) -> (195,207)
        if (state == 1) {
            return Rectangle{ 146.0f, 192.0f, 16.0f, 16.0f };
        } else if (state == 2) {
            return Rectangle{ 163.0f, 192.0f, 16.0f, 16.0f };
        } else {
            return Rectangle{ 180.0f, 192.0f, 16.0f, 16.0f };
        }
    }
}

void Fireball::update(float dt) {
    // Keep horizontal speed
    velocity.x = facingRight ? speed : -speed;

    if (fireballType == FireballType::Fireball1) {
        animTimer += dt;
        if (animTimer >= 0.08f) {
            animTimer = 0.0f;
            currentFrame = (currentFrame + 1) % 2;
        }
    } else {
        stateTimer += dt;
        if (state == 1) {
            if (stateTimer >= 0.7f) {
                state = 2;
                stateTimer = 0.0f;
            }
        } else if (state == 2) {
            if (stateTimer >= 1.0f) {
                state = 3;
                stateTimer = 0.0f;
            }
        } else if (state == 3) {
            if (stateTimer >= 1.5f) {
                explode();
                return;
            }
        }
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
        if (fireballType == FireballType::Fireball1) {
            explode();
        }
        return;
    }

    if (other.isSolid()) {
        if (side == CollisionSide::Bottom) {
            // Bounce upward
            velocity.y = -bounceForce;
            onGround = false;
            if (fireballType == FireballType::Fireball1) {
                bouncesLeft--;
                if (bouncesLeft <= 0) {
                    explode();
                }
            }
        } else if (side == CollisionSide::Left || side == CollisionSide::Right) {
            if (fireballType == FireballType::Fireball1) {
                explode();
            } else {
                // Fireball 2 ricochets off walls
                facingRight = !facingRight;
                velocity.x = facingRight ? speed : -speed;
            }
        } else if (side == CollisionSide::Top) {
            if (fireballType == FireballType::Fireball1) {
                explode();
            }
        }
    }
}

