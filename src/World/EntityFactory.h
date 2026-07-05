#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "Entities/Entity.h"
#include <memory>
#include <string>

class EntityFactory {
public:
    static std::unique_ptr<Entity> createEntity(char type, float x, float y);
};

#endif // ENTITY_FACTORY_H
