#include "StaticEntity.hpp"

StaticEntity::StaticEntity(Vector2 pos, Vector2 sprSize, Vector2 hitSize, Vector2 hitOffset, const std::string& texID, Color dbgColor)
    : Entity(pos, sprSize, hitSize, hitOffset, texID, dbgColor), solid(true) {}
