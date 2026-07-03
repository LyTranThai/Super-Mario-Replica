#include "EntityFactory.h"
#include "Entities/Block.h"
#include "Entities/InteractiveBlock.h"
#include "Entities/Goomba.h"
#include "Entities/Koopa.h"
#include "Entities/PiranhaPlant.h"
#include "Entities/RockHead.h"

std::unique_ptr<Entity> EntityFactory::createEntity(char type, float x, float y) {
    Vector2 pos = { x, y };
    switch (type) {
        case '#': // Solid ground Block
            return std::make_unique<Block>(pos, "solid", BROWN);
        
        case 'B': // Breakable brick block
            return std::make_unique<InteractiveBlock>(pos, InteractiveBlockType::Brick);
        
        case '?': // Question Block with Coin
            return std::make_unique<InteractiveBlock>(pos, InteractiveBlockType::Question, ItemType::Coin);
        
        case 'M': // Question Block with Mushroom
            return std::make_unique<InteractiveBlock>(pos, InteractiveBlockType::Question, ItemType::Mushroom);
        
        case 'F': // Question Block with FireFlower
            return std::make_unique<InteractiveBlock>(pos, InteractiveBlockType::Question, ItemType::FireFlower);
        
        case 'S': // Question Block with Star
            return std::make_unique<InteractiveBlock>(pos, InteractiveBlockType::Question, ItemType::Star);
        
        case 'G': // Goomba Enemy
            return std::make_unique<Goomba>(pos);
        
        case 'K': // Koopa Enemy
            return std::make_unique<Koopa>(pos);
        
        case 'T': // Thwomp / RockHead Enemy
            return std::make_unique<RockHead>(pos);
        
        case 'I': // Piranha Plant Enemy
            return std::make_unique<PiranhaPlant>(pos);
            
        default:
            return nullptr;
    }
}
