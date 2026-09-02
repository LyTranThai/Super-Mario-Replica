#include "EntityFactory.h"
#include "Entities/Block.h"
#include "Entities/InteractiveBlock.h"
#include "Entities/QuestionBlock.h"
#include "Entities/CoinBlock.h"
#include "Entities/BreakableBlock.h"
#include "Entities/Goomba.h"
#include "Entities/Koopa.h"
#include "Entities/PiranhaPlant.h"
#include "Entities/RockHead.h"
#include "Entities/MovingPlatform.h"
#include "Entities/ExitBlock.h"
#include "Entities/TeleportPipe.h"

std::unique_ptr<Entity> EntityFactory::createEntity(char type, float x, float y) {
    Vector2 pos = { x, y };
    switch (type) {
        case '#': // Solid ground Block
            return std::make_unique<Block>(pos, Block::Type::Ground, true, "world", WHITE);
            
        case 'X': // Normal brick (for stairs/walls)
            return std::make_unique<Block>(pos, Block::Type::Brick, true, "world", WHITE);
        
        case 'B': // Breakable brick block
            return std::make_unique<BreakableBlock>(pos);
        
        case '?': // Question Block with Coin
            return std::make_unique<CoinBlock>(pos, 1);
        
        case 'M': // Question Block with Mushroom
            return std::make_unique<QuestionBlock>(pos, ItemType::Mushroom);
        
        case 'F': // Question Block with FireFlower
            return std::make_unique<QuestionBlock>(pos, ItemType::FireFlower);
        
        case 'L': // Question Block with Heart (L for Life)
            return std::make_unique<QuestionBlock>(pos, ItemType::Heart);
            
        case 'S': // Question Block with Star
            return std::make_unique<QuestionBlock>(pos, ItemType::Star);
        
                
        case 'G': // Goomba Enemy
            return std::make_unique<Goomba>(pos);
        
        case 'K': // Koopa Enemy (Moved from K)
            return std::make_unique<Koopa>(pos);
        
        case 'T': // Thwomp / RockHead Enemy
            return std::make_unique<RockHead>(pos);
        
        case 'N': // Piranha Plant Enemy (Moved from I)
            return std::make_unique<PiranhaPlant>(pos);

        case '1': // Short Pipe
            return std::make_unique<TeleportPipe>(pos, PipeSize::Short);
        
        case '2': // Medium Pipe
            return std::make_unique<TeleportPipe>(pos, PipeSize::Medium);
            
        case '3': // Long Pipe
            return std::make_unique<TeleportPipe>(pos, PipeSize::Long);

        case 'H': // Horizontal moving platform
            return std::make_unique<MovingPlatform>(
                pos,
                Vector2{ 64.0f, 16.0f },
                std::make_unique<HorizontalMovingStrategy>(
                    x, x + 128.0f, 70.0f));

        case 'V': // Vertical moving platform
            return std::make_unique<MovingPlatform>(
                pos,
                Vector2{ 64.0f, 16.0f },
                std::make_unique<VerticalMovingStrategy>(
                    y - 96.0f, y, 55.0f, -1));

        case 'E': // Exit Block / Pipe
        case 'W': // Warp Pipe Exit Goal
            return std::make_unique<ExitBlock>(pos);
            
        default:
            return nullptr;
    }
}
