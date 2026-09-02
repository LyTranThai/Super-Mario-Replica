#include "EntitySpawner.h"
#include "World/Level.h"
#include "Entities/Item.h"
#include "ItemFactory.h"
#include "Entities/Fireball.h"
#include "Entities/InteractiveBlock.h" // For ItemSpawnData
#include <iostream>

EntitySpawner::EntitySpawner() : currentLevel(nullptr) {
    EventManager::getInstance().subscribe(EventType::ItemSpawned, this);
    EventManager::getInstance().subscribe(EventType::FireballShot, this);
}

EntitySpawner::~EntitySpawner() {
    EventManager::getInstance().unsubscribe(EventType::ItemSpawned, this);
    EventManager::getInstance().unsubscribe(EventType::FireballShot, this);
}

void EntitySpawner::setLevel(Level* level) {
    currentLevel = level;
}

void EntitySpawner::onEvent(EventType type, void* data) {
    if (!currentLevel || !data) return;

    if (type == EventType::ItemSpawned) {
        ItemSpawnData* spawn = static_cast<ItemSpawnData*>(data);
        auto item = ItemFactory::createItem(spawn->type, spawn->position);
        if (item) {
            currentLevel->spawnEntity(std::move(item));
        }
    } else if (type == EventType::FireballShot) {
        FireballSpawnData* spawn = static_cast<FireballSpawnData*>(data);
        currentLevel->spawnEntity(std::make_unique<Fireball>(spawn->position, spawn->facingRight, spawn->type));
    }
}
