#ifndef STATIC_ENTITY_H
#define STATIC_ENTITY_H

#include "Entity.h"

class Player;

class StaticEntity : public Entity {
protected:
    bool solid;

public:
    StaticEntity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor = BROWN);
    virtual ~StaticEntity() override = default;
    
    bool isSolid() const override { return solid; }
    void setSolid(bool state) { solid = state; }

    virtual void onInteract(Player& player) = 0;
};

#endif // STATIC_ENTITY_H
