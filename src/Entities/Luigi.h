#ifndef LUIGI_H
#define LUIGI_H

#include "FireballPlayer.h"

class Luigi : public FireballPlayer {
public:
    Luigi(Vector2 pos);
    void configureAnimations() override;
    Color getSpriteTint() const override;
    Texture2D getSpriteTexture() const override;
};

#endif // LUIGI_H
