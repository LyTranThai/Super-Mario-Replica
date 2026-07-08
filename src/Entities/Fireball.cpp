#include "Fireball.hpp"
#include "Enemy.hpp"
#include <iostream>

Fireball::Fireball(Vector2 pos, bool faceRight)
    : DynamicEntity(pos, Vector2{ 16.0f, 16.0f }, Vector2{ 12.0f, 12.0f }, Vector2{ 2.0f, 2.0f }, "fireball", ORANGE),
      bounceForce(250.0f), speed(350.0f), bouncesLeft(4) {
    facingRight = faceRight;
    velocity.x = facingRight ? speed : -speed;
    onGround = false;
}

void Fireball::update(float dt) {
    (void)dt;
    // Set horizontal speed continuously
    velocity.x = facingRight ? speed : -speed;
}

void Fireball::explode() {
    active = false;
    // Trigger explode visual effect or sound if desired
}

void Fireball::onCollision(Entity& other, CollisionSide side) {
    if (!other.isActive()) return;

    Enemy* enemy = dynamic_cast<Enemy*>(&other);
    if (enemy) {
        // Hit enemy -> kills enemy and destroys fireball
        enemy->takeDamage();
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
            // Hit wall or ceiling -> explodes immediately
            explode();
        }
    }
}
