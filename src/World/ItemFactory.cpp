#include "ItemFactory.h"
#include "Entities/Mushroom.h"
#include "Entities/FireFlower.h"
#include "Entities/Heart.h"
#include "Entities/Star.h"
#include "Entities/VisualCoin.h"

std::unique_ptr<Item> ItemFactory::createItem(ItemType type, Vector2 pos) {
    switch (type) {
        case ItemType::Mushroom:
            return std::make_unique<Mushroom>(pos);
        case ItemType::FireFlower:
            return std::make_unique<FireFlower>(pos);
        case ItemType::Heart:
            return std::make_unique<Heart>(pos);
        case ItemType::Star:
            return std::make_unique<Star>(pos);
        case ItemType::Coin:
            return std::make_unique<VisualCoin>(pos);
        default:
            return nullptr;
    }
}
