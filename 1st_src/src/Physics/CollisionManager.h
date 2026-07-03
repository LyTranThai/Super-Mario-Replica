#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "raylib.h"
#include <vector>
#include <memory>

class Entity;
class DynamicEntity;
class Player;

class CollisionManager {
public:
    CollisionManager() = default;

    static bool checkAABB(Rectangle r1, Rectangle r2);
    static float getOverlapX(Rectangle r1, Rectangle r2);
    static float getOverlapY(Rectangle r1, Rectangle r2);
    static bool isSolidCollision(const Entity& e1, const Entity& e2);

    // Primary update loop mapping physics, look-ahead sweeps, and collisions
    void updatePhysicsAndCollisions(std::vector<std::unique_ptr<Entity>>& entities, Player& player, float dt);

    // Stuck recovery push-out logic
    void resolveStuckRecovery(DynamicEntity& entity, const std::vector<std::unique_ptr<Entity>>& entities);
};

#endif // COLLISION_MANAGER_H
