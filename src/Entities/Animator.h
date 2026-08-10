#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "raylib.h"
#include <string>

class Animator {
public:
    virtual ~Animator() = default;
    virtual void update(float dt) = 0;
    virtual Rectangle getCurrentFrame() const = 0;
    virtual void clearAnimations() = 0;
    
    // Core state management
    virtual void setState(const std::string& state) = 0;
    virtual std::string getState() const = 0;
};

#endif // ANIMATOR_H
