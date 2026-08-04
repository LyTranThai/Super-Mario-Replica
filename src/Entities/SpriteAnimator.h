#ifndef SPRITE_ANIMATOR_H
#define SPRITE_ANIMATOR_H

#include "raylib.h"
#include <vector>
#include <unordered_map>

// Animation state names for the player
enum class AnimState {
    Idle,
    Walk,
    Jump,
    Fall,
    Crouch,
    Die,
    Skid
};

// A single animation: a sequence of source-rectangle frames on a spritesheet
struct Animation {
    std::vector<Rectangle> frames;  // Source rects on the spritesheet
    float frameTime;                // Seconds per frame
    bool loop;                      // Whether to loop or freeze on last frame

    Animation() : frameTime(0.1f), loop(true) {}
    Animation(std::vector<Rectangle> f, float ft, bool lp = true)
        : frames(std::move(f)), frameTime(ft), loop(lp) {}
};

// Drives frame-based sprite animation from a spritesheet
class SpriteAnimator {
private:
    // Use int as key for unordered_map compatibility
    std::unordered_map<int, Animation> animations;
    AnimState currentState;
    int currentFrame;
    float timer;

public:
    SpriteAnimator() : currentState(AnimState::Idle), currentFrame(0), timer(0.0f) {}

    void addAnimation(AnimState state, const std::vector<Rectangle>& frames, float frameTime = 0.1f, bool loop = true) {
        animations[static_cast<int>(state)] = Animation(frames, frameTime, loop);
    }

    void setState(AnimState state) {
        if (state != currentState) {
            currentState = state;
            currentFrame = 0;
            timer = 0.0f;
        }
    }

    void update(float dt) {
        auto it = animations.find(static_cast<int>(currentState));
        if (it == animations.end() || it->second.frames.empty()) return;

        const Animation& anim = it->second;
        timer += dt;

        if (timer >= anim.frameTime) {
            timer -= anim.frameTime;
            if (anim.loop) {
                currentFrame = (currentFrame + 1) % (int)anim.frames.size();
            } else {
                if (currentFrame < (int)anim.frames.size() - 1) {
                    currentFrame++;
                }
            }
        }
    }

    // Returns the source rectangle on the spritesheet for the current frame
    Rectangle getCurrentFrame() const {
        auto it = animations.find(static_cast<int>(currentState));
        if (it == animations.end() || it->second.frames.empty()) {
            return Rectangle{ 0, 0, 16, 16 }; // fallback
        }
        const auto& frames = it->second.frames;
        int idx = currentFrame < (int)frames.size() ? currentFrame : 0;
        return frames[idx];
    }

    AnimState getState() const { return currentState; }

    // Clear all animations (used when switching power states)
    void clearAnimations() {
        animations.clear();
        currentFrame = 0;
        timer = 0.0f;
    }
};

#endif // SPRITE_ANIMATOR_H
