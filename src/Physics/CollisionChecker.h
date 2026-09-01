#ifndef COLLISION_CHECKER_H
#define COLLISION_CHECKER_H

#include "raylib.h"
#include <vector>
#include <memory>
#include "Entities/Entity.h"

class Player;
class DynamicEntity;

class CollisionChecker {
public:
    CollisionChecker() = default;
    ~CollisionChecker() = default;

    static bool checkAABB(Rectangle r1, Rectangle r2);
    static float getOverlapX(Rectangle r1, Rectangle r2);
    static float getOverlapY(Rectangle r1, Rectangle r2);

    void updatePhysics(std::vector<std::unique_ptr<Entity>>& entities, std::vector<std::unique_ptr<Player>>& players, float dt);
    void updatePhysics(std::vector<std::unique_ptr<Entity>>& entities, Player& player, float dt);
    static void checkPlayerPlayerCollision(Player& p1, Player& p2);

private:
    void sweepEntity(DynamicEntity* dyn, const std::vector<std::unique_ptr<Entity>>& entities, const std::vector<std::unique_ptr<Player>>& players, float dt);
    void sweepEntity(DynamicEntity* dyn, const std::vector<std::unique_ptr<Entity>>& entities, Player* player, float dt);
};

#endif // COLLISION_CHECKER_H
