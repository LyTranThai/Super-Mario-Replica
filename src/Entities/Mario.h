#ifndef MARIO_H
#define MARIO_H

#include "FireballPlayer.h"

class Mario : public FireballPlayer {
public:
    Mario(Vector2 pos);
    void configureAnimations() override;
};

#endif // MARIO_H
