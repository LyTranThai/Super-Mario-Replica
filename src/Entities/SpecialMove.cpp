#include "SpecialMove.h"
#include "Player.h"
#include "Core/EventSystem.h"
#include <iostream>
#include "Fireball.h"
#include "FireballPlayer.h"

void FireballMove::execute(Player& player) {
    if (auto* fbPlayer = dynamic_cast<FireballPlayer*>(&player)) {
        fbPlayer->resetPowerMoveCooldown(2.0f); // 2 seconds cooldown
    }
    std::cout << "[DEBUG] " << player.getTextureID() << " shoot fireball from SpecialMove" << std::endl;
    
    Vector2 spawnPos;
    float fbWidth = 16.0f;
    float fbHeight = 16.0f;
    
    Vector2 position = player.getPosition();
    Vector2 hitboxOffset = player.getHitboxOffset();
    Vector2 hitboxSize = player.getHitboxSize();
    bool facingRight = player.isFacingRight();

    if (facingRight)
    {
        spawnPos.x = position.x + hitboxOffset.x + hitboxSize.x + 2.0f;
    }
    else
    {
        spawnPos.x = position.x + hitboxOffset.x - fbWidth - 2.0f;
    }
    spawnPos.y = position.y + hitboxOffset.y + (hitboxSize.y / 2.0f) - (fbHeight / 2.0f);

    FireballSpawnData data = {spawnPos, facingRight, FireballType::Fireball1};
    EventManager::getInstance().broadcast(EventType::FireballShot, &data);
}
