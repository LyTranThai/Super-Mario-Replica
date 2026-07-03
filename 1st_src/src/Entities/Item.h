#ifndef ITEM_H
#define ITEM_H

#include "DynamicEntity.h"
#include "InteractiveBlock.h"

class Item : public DynamicEntity {
protected:
    ItemType itemType;
    float spawnRiseTimer; // Visual effect popping out of block
    Vector2 targetSpawnPosition;

public:
    Item(Vector2 pos, ItemType type, const std::string& texID, Color dbgColor);
    virtual ~Item() override = default;

    void update(float dt) override;
    void onCollision(Entity& other, CollisionSide side) override;

    ItemType getItemType() const { return itemType; }
};

#endif // ITEM_H
