#include "CollisionChecker.h"
#include "Entities/DynamicEntity.h"
#include "Entities/MovingPlatform.h"
#include "Entities/Player.h"
#include "Entities/InteractiveBlock.h"
#include <cmath>

bool CollisionChecker::checkAABB(Rectangle r1, Rectangle r2) {
    return (r1.x < r2.x + r2.width  && r1.x + r1.width > r2.x &&
            r1.y < r2.y + r2.height && r1.y + r1.height > r2.y);
}

float CollisionChecker::getOverlapX(Rectangle r1, Rectangle r2) {
    float r1Right = r1.x + r1.width;
    float r2Right = r2.x + r2.width;
    
    if (r1.x < r2.x) {
        return r1Right - r2.x;
    } else {
        return r2Right - r1.x;
    }
}

float CollisionChecker::getOverlapY(Rectangle r1, Rectangle r2) {
    float r1Bottom = r1.y + r1.height;
    float r2Bottom = r2.y + r2.height;
    
    if (r1.y < r2.y) {
        return r1Bottom - r2.y;
    } else {
        return r2Bottom - r1.y;
    }
}

void CollisionChecker::sweepEntity(DynamicEntity* dyn, const std::vector<std::unique_ptr<Entity>>& entities, Player* player, float dt) {
    if (!dyn || !dyn->isActive() || dyn->isCarried()) return;
    
    dyn->applyGravity(dt);

    Vector2 pos = dyn->getPosition();
    Vector2 vel = dyn->getVelocity();

    // Helper lambda to process a single overlapping entity
    auto processOverlap = [&](Entity* other, bool isHorizontal) {
        if (!other || !other->isActive() || other == dyn) return;
        
        Rectangle dBox = dyn->getBoundingBox();
        Rectangle oBox = other->getBoundingBox();
        
        if (isHorizontal) {
            if (checkAABB(dBox, oBox)) {
                float overlapX = getOverlapX(dBox, oBox);
                float overlapY = getOverlapY(dBox, oBox);
                
                if (overlapY > 0.1f) {
                    CollisionSide hitSide = (vel.x > 0.0f) ? CollisionSide::Right : CollisionSide::Left;
                    dyn->resolveOverlap(*other, overlapX, hitSide);
                    vel = dyn->getVelocity();
                    pos = dyn->getPosition();
                }
            }
        } else {
            Rectangle dBoxDown = dBox;
            dBoxDown.height += 1.0f;
            
            if (checkAABB(dBoxDown, oBox)) {
                float overlapY = getOverlapY(dBox, oBox);
                float overlapX = getOverlapX(dBoxDown, oBox);
                
                if (overlapX > 0.1f) {
                    if (vel.y >= 0.0f) {
                        if (dBox.y + dBox.height / 2.0f < oBox.y + oBox.height / 2.0f) {
                            dyn->resolveOverlap(*other, overlapY, CollisionSide::Bottom);
                        }
                    } else if (vel.y < 0.0f) {
                        if (checkAABB(dBox, oBox)) {
                            if (dBox.y + dBox.height / 2.0f > oBox.y + oBox.height / 2.0f) {
                                dyn->resolveOverlap(*other, overlapY, CollisionSide::Top);
                                InteractiveBlock* block = dynamic_cast<InteractiveBlock*>(other);
                                Player* p = dynamic_cast<Player*>(dyn);
                                if (block && p) {
                                    block->onInteract(*p);
                                }
                            }
                        }
                    }
                    vel = dyn->getVelocity();
                    pos = dyn->getPosition();
                }
            }
        }
    };

    // --- Move horizontally ---
    pos.x += vel.x * dt;
    dyn->setPosition(pos);
    
    for (auto const& other : entities) processOverlap(other.get(), true);
    if (player) processOverlap(player, true);

    // --- Move vertically ---
    pos.y += vel.y * dt;
    dyn->setPosition(pos);
    dyn->setOnGround(false);
    
    for (auto const& other : entities) processOverlap(other.get(), false);
    if (player) processOverlap(player, false);
}

void CollisionChecker::updatePhysics(std::vector<std::unique_ptr<Entity>>& entities, Player& player, float dt) {
    // Platforms update before physics. Carry a rider by the exact distance the
    // platform moved this frame, using its previous bounds for contact.
    if (player.isActive() && player.getVelocity().y >= 0.0f) {
        Rectangle playerBounds = player.getBoundingBox();
        for (auto const& entity : entities) {
            MovingPlatform* platform = dynamic_cast<MovingPlatform*>(entity.get());
            if (!platform || !platform->isActive()) {
                continue;
            }

            if (platform->wasStandingOn(playerBounds)) {
                Vector2 playerPosition = player.getPosition();
                Vector2 platformMovement = platform->getFrameMovement();
                playerPosition.x += platformMovement.x;
                playerPosition.y += platformMovement.y;
                player.setPosition(playerPosition);
                break;
            }
        }
    }

    // Handle Player stand up logic first
    if (player.isActive() && player.getWantToStandUp()) {
        Vector2 baseSpriteSize, baseHitboxSize, baseHitboxOffset;
        player.getPowerStateDimensions(baseSpriteSize, baseHitboxSize, baseHitboxOffset);
        
        Rectangle stoodUpBox = {
            player.getPosition().x + baseHitboxOffset.x,
            player.getPosition().y + baseHitboxOffset.y,
            baseHitboxSize.x,
            baseHitboxSize.y
        };
        
        bool ceilingBlocked = false;
        for (auto const& other : entities) {
            if (!other->isActive() || other.get() == &player) continue;
            if (other->isSolidFrom(CollisionSide::Top, &player) && checkAABB(stoodUpBox, other->getBoundingBox())) {
                ceilingBlocked = true;
                break;
            }
        }
        
        if (!ceilingBlocked) {
            player.setCrouching(false);
            player.applyHitboxDimensions();
        }
        player.setWantToStandUp(false);
    }

    // 1. Sweep Player
    if (player.isActive()) {
        sweepEntity(&player, entities, nullptr, dt);
    }

    // 2. Sweep all other DynamicEntities
    for (auto& entity : entities) {
        DynamicEntity* dyn = dynamic_cast<DynamicEntity*>(entity.get());
        if (dyn) {
            sweepEntity(dyn, entities, &player, dt);
        }
    }
}
