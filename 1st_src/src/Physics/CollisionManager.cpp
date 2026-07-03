#include "CollisionManager.h"
#include "Entities/Entity.h"
#include "Entities/DynamicEntity.h"
#include "Entities/Player.h"
#include <cmath>
#include <iostream>

bool CollisionManager::checkAABB(Rectangle r1, Rectangle r2) {
    return (r1.x < r2.x + r2.width  && r1.x + r1.width > r2.x &&
            r1.y < r2.y + r2.height && r1.y + r1.height > r2.y);
}

float CollisionManager::getOverlapX(Rectangle r1, Rectangle r2) {
    float r1Right = r1.x + r1.width;
    float r2Right = r2.x + r2.width;
    
    if (r1.x < r2.x) {
        return r1Right - r2.x;
    } else {
        return r2Right - r1.x;
    }
}

float CollisionManager::getOverlapY(Rectangle r1, Rectangle r2) {
    float r1Bottom = r1.y + r1.height;
    float r2Bottom = r2.y + r2.height;
    
    if (r1.y < r2.y) {
        return r1Bottom - r2.y;
    } else {
        return r2Bottom - r1.y;
    }
}

void CollisionManager::resolveStuckRecovery(DynamicEntity& entity, const std::vector<std::unique_ptr<Entity>>& entities) {
    Rectangle eBox = entity.getBoundingBox();

    for (auto const& other : entities) {
        if (!other->isActive() || !other->isSolid() || other.get() == &entity) {
            continue;
        }

        Rectangle oBox = other->getBoundingBox();
        if (checkAABB(eBox, oBox)) {
            // Overlapping! Push out along shortest axis
            float overlapX = getOverlapX(eBox, oBox);
            float overlapY = getOverlapY(eBox, oBox);

            Vector2 pos = entity.getPosition();
            Vector2 size = entity.getHitboxSize();

            if (overlapX < overlapY) {
                // Push out horizontally
                if (eBox.x + eBox.width / 2.0f < oBox.x + oBox.width / 2.0f) {
                    pos.x -= overlapX; // Push left
                    if (entity.getVelocity().x > 0.0f) {
                        entity.setVelocity(Vector2{ 0.0f, entity.getVelocity().y });
                    }
                } else {
                    pos.x += overlapX; // Push right
                    if (entity.getVelocity().x < 0.0f) {
                        entity.setVelocity(Vector2{ 0.0f, entity.getVelocity().y });
                    }
                }
                entity.setPosition(pos);
            } else {
                // Push out vertically
                if (eBox.y + eBox.height / 2.0f < oBox.y + oBox.height / 2.0f) {
                    pos.y -= overlapY; // Push up
                    entity.setOnGround(true);
                    if (entity.getVelocity().y > 0.0f) {
                        entity.setVelocity(Vector2{ entity.getVelocity().x, 0.0f });
                    }
                } else {
                    pos.y += overlapY; // Push down
                    if (entity.getVelocity().y < 0.0f) {
                        entity.setVelocity(Vector2{ entity.getVelocity().x, 0.0f });
                    }
                }
                entity.setPosition(pos);
            }
            eBox = entity.getBoundingBox(); // Update tracking box
        }
    }
}

void CollisionManager::updatePhysicsAndCollisions(std::vector<std::unique_ptr<Entity>>& entities, Player& player, float dt) {
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
            if (!other->isActive() || !other->isSolid() || other.get() == &player) {
                continue;
            }
            if (checkAABB(stoodUpBox, other->getBoundingBox())) {
                ceilingBlocked = true;
                break;
            }
        }
        
        if (!ceilingBlocked) {
            player.setCrouching(false);
            player.applyHitboxDimensions();
        }
        // Always reset stand up request immediately
        player.setWantToStandUp(false);
    }

    // 1. Stuck recovery run first to ensure entities start in valid positions
    resolveStuckRecovery(player, entities);
    for (auto& entity : entities) {
        if (entity->isActive() && entity.get() != &player) {
            DynamicEntity* dyn = dynamic_cast<DynamicEntity*>(entity.get());
            if (dyn) {
                resolveStuckRecovery(*dyn, entities);
            }
        }
    }

    // 2. Player horizontal look-ahead update
    if (player.isActive()) {
        player.applyGravity(dt);

        Vector2 playerPos = player.getPosition();
        Vector2 playerVel = player.getVelocity();
        Vector2 playerSize = player.getHitboxSize();

        // Move horizontally
        playerPos.x += playerVel.x * dt;
        player.setPosition(playerPos);

        // Check horizontal collisions
        Rectangle pBox = player.getBoundingBox();
        for (auto const& other : entities) {
            if (!other->isActive() || !other->isSolid()) continue;
            
            Rectangle oBox = other->getBoundingBox();
            if (checkAABB(pBox, oBox)) {
                // Collision! Reset position and set velocity to 0
                float overlap = getOverlapX(pBox, oBox);
                if (playerVel.x > 0.0f) {
                    playerPos.x -= overlap; // Hit wall on right
                    player.onCollision(*other, CollisionSide::Right);
                } else if (playerVel.x < 0.0f) {
                    playerPos.x += overlap; // Hit wall on left
                    player.onCollision(*other, CollisionSide::Left);
                }
                playerVel.x = 0.0f;
                player.setPosition(playerPos);
                pBox = player.getBoundingBox();
            }
        }

        // Move vertically
        playerPos.y += playerVel.y * dt;
        player.setPosition(playerPos);
        pBox = player.getBoundingBox();
        player.setOnGround(false); // Reset onGround state before check

        // Check vertical collisions
        for (auto const& other : entities) {
            if (!other->isActive() || !other->isSolid()) continue;
            
            Rectangle oBox = other->getBoundingBox();
            
            // Look-ahead slightly downward to detect floor even if exactly flush
            Rectangle pBoxDown = pBox;
            pBoxDown.height += 1.0f;
            
            if (checkAABB(pBoxDown, oBox)) {
                float overlap = getOverlapY(pBox, oBox);
                if (playerVel.y >= 0.0f) {
                    // Only land if player is above the block center
                    if (pBox.y + pBox.height / 2.0f < oBox.y + oBox.height / 2.0f) {
                        playerPos.y -= overlap; // Landed on ground
                        playerVel.y = 0.0f;
                        player.setOnGround(true);
                        player.onCollision(*other, CollisionSide::Bottom);
                    }
                } else if (playerVel.y < 0.0f) {
                    // For ceiling checks, verify with original pBox to avoid false ceiling triggers
                    if (checkAABB(pBox, oBox)) {
                        if (pBox.y + pBox.height / 2.0f > oBox.y + oBox.height / 2.0f) {
                            playerPos.y += overlap; // Hit ceiling
                            playerVel.y = 0.0f;
                            player.onCollision(*other, CollisionSide::Top);
                        }
                    }
                }
                player.setPosition(playerPos);
                pBox = player.getBoundingBox();
            }
        }
        player.setVelocity(playerVel);
    }

    // 3. Enemies and other Dynamic Entities look-ahead update
    for (auto& entity : entities) {
        if (!entity->isActive() || entity.get() == &player) continue;

        DynamicEntity* dyn = dynamic_cast<DynamicEntity*>(entity.get());
        if (!dyn) continue;

        dyn->applyGravity(dt);

        Vector2 pos = dyn->getPosition();
        Vector2 vel = dyn->getVelocity();

        // Move horizontally
        pos.x += vel.x * dt;
        dyn->setPosition(pos);

        Rectangle dBox = dyn->getBoundingBox();
        for (auto const& other : entities) {
            if (!other->isActive() || !other->isSolid() || other.get() == dyn) continue;
            
            Rectangle oBox = other->getBoundingBox();
            if (checkAABB(dBox, oBox)) {
                float overlap = getOverlapX(dBox, oBox);
                if (vel.x > 0.0f) {
                    pos.x -= overlap;
                    dyn->onCollision(*other, CollisionSide::Right);
                } else if (vel.x < 0.0f) {
                    pos.x += overlap;
                    dyn->onCollision(*other, CollisionSide::Left);
                }
                vel = dyn->getVelocity(); // Reload velocity in case onCollision reversed it
                dyn->setPosition(pos);
                dBox = dyn->getBoundingBox();
            }
        }

        // Move vertically
        pos.y += vel.y * dt;
        dyn->setPosition(pos);
        dBox = dyn->getBoundingBox();
        dyn->setOnGround(false);

        for (auto const& other : entities) {
            if (!other->isActive() || !other->isSolid() || other.get() == dyn) continue;
            
            Rectangle oBox = other->getBoundingBox();
            
            // Look-ahead slightly downward to detect floor even if exactly flush
            Rectangle dBoxDown = dBox;
            dBoxDown.height += 1.0f;
            
            if (checkAABB(dBoxDown, oBox)) {
                float overlap = getOverlapY(dBox, oBox);
                if (vel.y >= 0.0f) {
                    // Only land if enemy is above the block center
                    if (dBox.y + dBox.height / 2.0f < oBox.y + oBox.height / 2.0f) {
                        pos.y -= overlap;
                        vel.y = 0.0f;
                        dyn->setOnGround(true);
                        dyn->onCollision(*other, CollisionSide::Bottom);
                    }
                } else if (vel.y < 0.0f) {
                    // For ceiling checks, verify with original dBox to avoid false ceiling triggers
                    if (checkAABB(dBox, oBox)) {
                        if (dBox.y + dBox.height / 2.0f > oBox.y + oBox.height / 2.0f) {
                            pos.y += overlap;
                            vel.y = 0.0f;
                            dyn->onCollision(*other, CollisionSide::Top);
                        }
                    }
                }
                dyn->setPosition(pos);
                dBox = dyn->getBoundingBox();
            }
        }
        dyn->setVelocity(vel);
    }

    // 4. Dynamic vs Dynamic Interactions (Overlap based trigger callbacks)
    if (player.isActive()) {
        Rectangle pBox = player.getBoundingBox();
        for (auto& entity : entities) {
            if (!entity->isActive() || entity.get() == &player) continue;

            Rectangle oBox = entity->getBoundingBox();
            if (checkAABB(pBox, oBox)) {
                DynamicEntity* dyn = dynamic_cast<DynamicEntity*>(entity.get());
                if (dyn) {
                    // Check if player stomps enemy (falling down and bottom of player is above middle of enemy)
                    bool isFalling = (player.getVelocity().y > 0.0f);
                    float playerBottom = pBox.y + pBox.height;
                    float enemyMiddle = oBox.y + oBox.height / 2.0f;

                    if (isFalling && playerBottom < enemyMiddle) {
                        // Stomp! Bounce player upward
                        player.setVelocity(Vector2{ player.getVelocity().x, -350.0f });
                        dyn->onCollision(player, CollisionSide::Top);
                        player.onCollision(*dyn, CollisionSide::Bottom);
                    } else {
                        // Regular hit
                        dyn->onCollision(player, CollisionSide::None);
                        player.onCollision(*dyn, CollisionSide::None);
                    }
                }
            }
        }
    }

    // Projectile vs Enemy collisions (Fireball vs Goomba/Koopa)
    // We will let each fireball check collision against enemies in its own update method,
    // or evaluate it here dynamically. Checking it inside update is simple, but doing it here matches central management.
    for (auto& e1 : entities) {
        if (!e1->isActive()) continue;
        
        // Check if e1 is fireball
        if (e1->isActive() && e1->getSpriteSize().x == 16.0f && e1->getHitboxSize().x == 12.0f) { // Simple check for Fireball properties
            Rectangle fBox = e1->getBoundingBox();
            for (auto& e2 : entities) {
                if (!e2->isActive() || e2.get() == e1.get() || e2.get() == &player) continue;
                
                // If e2 is enemy
                DynamicEntity* enemy = dynamic_cast<DynamicEntity*>(e2.get());
                if (enemy && !e2->isSolid()) { // If it's a non-solid active NPC (Goomba/Koopa)
                    if (checkAABB(fBox, enemy->getBoundingBox())) {
                        e1->setActive(false); // Destroy fireball
                        enemy->onCollision(*e1, CollisionSide::None); // Kill enemy
                    }
                }
            }
        }
    }
}
