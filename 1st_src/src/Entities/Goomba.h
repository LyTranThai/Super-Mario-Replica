#ifndef GOOMBA_H
#define GOOMBA_H

#include "Enemy.h"

class Goomba : public Enemy {
public:
    Goomba(Vector2 pos);
    ~Goomba() override = default;
};

#endif // GOOMBA_H
