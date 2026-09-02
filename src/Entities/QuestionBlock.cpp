#include "QuestionBlock.h"
#include "Player.h"
#include "Core/EventSystem.h"

QuestionBlock::QuestionBlock(Vector2 pos, ItemType item)
    : InteractiveBlock(pos, InteractiveBlockType::Question), hiddenItem(item) {}

void QuestionBlock::hit(Player& player) {
    if (!isUsed) {
        isUsed = true;
        bounceTimer = 0.15f;

        if (hiddenItem != ItemType::None) {
            // Spawn item popping out upward
            Vector2 spawnPos = { originalPosition.x, originalPosition.y - 17.0f };
            ItemSpawnData spawnData = { spawnPos, hiddenItem };
            EventManager::getInstance().broadcast(EventType::ItemSpawned, &spawnData);
        }
    }
}
