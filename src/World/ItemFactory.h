#ifndef ITEM_FACTORY_H
#define ITEM_FACTORY_H

#include <memory>
#include "raylib.h"
#include "Entities/Item.h"
#include "Entities/InteractiveBlock.h" // For ItemType

class ItemFactory {
public:
    static std::unique_ptr<Item> createItem(ItemType type, Vector2 pos);
};

#endif // ITEM_FACTORY_H
