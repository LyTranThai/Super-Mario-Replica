#include "SpecialMove.hpp"
#include "Player.hpp"

void FireballMove::execute(Player& player) {
    player.shootFireball();
}
