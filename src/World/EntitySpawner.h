#ifndef ENTITY_SPAWNER_H
#define ENTITY_SPAWNER_H

#include "Core/EventSystem.h"
#include <memory>

class Level;

class EntitySpawner : public EventListener {
private:
    Level* currentLevel;

public:
    EntitySpawner();
    ~EntitySpawner() override;

    void setLevel(Level* level);
    void onEvent(EventType type, void* data = nullptr) override;
};

#endif // ENTITY_SPAWNER_H
