#ifndef PIRANHA_PLANT_H
#define PIRANHA_PLANT_H

#include "Enemy.hpp"

enum class PlantState {
    Hidden,
    Emerging,
    Extended,
    Retracting
};

class PiranhaPlant : public Enemy {
private:
    PlantState plantState;
    float timer;
    Vector2 pipePosition; // Base position (locked to top-middle of the pipe)
    float currentYOffset;

    static constexpr float maxOffset = 48.0f; // Height to slide up

public:
    PiranhaPlant(Vector2 pos);
    ~PiranhaPlant() override = default;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;
};

#endif // PIRANHA_PLANT_H
