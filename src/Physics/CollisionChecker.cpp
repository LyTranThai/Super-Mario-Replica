#include "CollisionChecker.h"
#include "Entities/DynamicEntity.h"
#include "Entities/MovingPlatform.h"
#include "Entities/Player.h"
#include "Entities/InteractiveBlock.h"
#include "Core/EventSystem.h"
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
                
                if (!other->isSolidFrom(CollisionSide::Right, dyn) && !other->isSolidFrom(CollisionSide::Left, dyn)) {
                    CollisionSide hitSide = (vel.x > 0.0f) ? CollisionSide::Right : (vel.x < 0.0f ? CollisionSide::Left : CollisionSide::None);
                    dyn->resolveOverlap(*other, overlapX, hitSide);
                } else if (std::abs(vel.x) > 0.001f && overlapY > 4.0f) {
                    float dCenterX = dBox.x + dBox.width / 2.0f;
                    float oCenterX = oBox.x + oBox.width / 2.0f;
                    if (vel.x > 0.0f && dCenterX < oCenterX) {
                        dyn->resolveOverlap(*other, overlapX, CollisionSide::Right);
                        vel = dyn->getVelocity();
                        pos = dyn->getPosition();
                    } else if (vel.x < 0.0f && dCenterX > oCenterX) {
                        dyn->resolveOverlap(*other, overlapX, CollisionSide::Left);
                        vel = dyn->getVelocity();
                        pos = dyn->getPosition();
                    }
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

    // --- Ledge detection ---
    if (dyn->avoidsCliffs() && dyn->isOnGround()) {
        Rectangle dBox = dyn->getBoundingBox();
        vel = dyn->getVelocity(); // Update vel in case processOverlap changed it
        
        Rectangle ledgeBox;
        ledgeBox.width = 2.0f;
        ledgeBox.height = 2.0f;
        ledgeBox.y = dBox.y + dBox.height + 1.0f; // Just below the bounding box
        
        if (vel.x > 0) {
            ledgeBox.x = dBox.x + dBox.width - 2.0f; // Front right edge
        } else if (vel.x < 0) {
            ledgeBox.x = dBox.x; // Front left edge
        } else {
            ledgeBox.width = 0; // Not moving, no check
        }
        
        if (ledgeBox.width > 0) {
            bool hasGround = false;
            for (auto const& other : entities) {
                if (other.get() != dyn && other->isSolidFrom(CollisionSide::Top, dyn)) {
                    if (checkAABB(ledgeBox, other->getBoundingBox())) {
                        hasGround = true;
                        break;
                    }
                }
            }
            
            if (!hasGround) {
                // Revert horizontal move and turn around
                pos = dyn->getPosition(); // get current position
                pos.x -= vel.x * dt;
                dyn->setPosition(pos);
                vel.x = -vel.x;
                dyn->setVelocity(vel);
                dyn->setFacingRight(vel.x > 0);
            }
        }
    }

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
    if (player.isActive() && !player.isPiping()) {
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

void CollisionChecker::checkPlayerPlayerCollision(Player& p1, Player& p2) {
    if (!p1.isActive() || !p2.isActive() || p1.isPiping() || p2.isPiping()) return;

    Rectangle r1 = p1.getBoundingBox();
    Rectangle r2 = p2.getBoundingBox();

    if (!checkAABB(r1, r2)) return;

    // Check if P1 is landing on P2's head
    float p1Bottom = r1.y + r1.height;
    float p2Top = r2.y;
    float p2Bottom = r2.y + r2.height;
    float p1Top = r1.y;

    if (p1.getVelocity().y > 0.0f && p1Bottom >= p2Top && p1Bottom <= p2Top + 14.0f) {
        // P1 bounces off P2's head!
        Vector2 v = p1.getVelocity();
        v.y = -460.0f; // Boost jump!
        p1.setVelocity(v);
        p1.setjumpCount(1);
        EventManager::getInstance().broadcast(EventType::EnemyStomped);
    } else if (p2.getVelocity().y > 0.0f && p2Bottom >= p1Top && p2Bottom <= p1Top + 14.0f) {
        // P2 bounces off P1's head!
        Vector2 v = p2.getVelocity();
        v.y = -460.0f; // Boost jump!
        p2.setVelocity(v);
        p2.setjumpCount(1);
        EventManager::getInstance().broadcast(EventType::EnemyStomped);
    }
}
