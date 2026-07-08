#ifndef GOOMBA_H
#define GOOMBA_H

#include "Enemy.hpp"

class Goomba : public Enemy {
public:
    Goomba(Vector2 pos);
    ~Goomba() override = default;
};

#endif // GOOMBA_H
