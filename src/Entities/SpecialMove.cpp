#include "SpecialMove.h"
#include "Player.h"

void FireballMove::execute(Player& player) {
    player.shootFireball();
}
