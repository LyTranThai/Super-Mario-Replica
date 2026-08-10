#ifndef SPRITE_ANIMATOR_H
#define SPRITE_ANIMATOR_H

#include "Animator.h"
#include <vector>
#include <unordered_map>
#include <string>

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
class SpriteAnimator : public Animator {
private:
    std::unordered_map<std::string, Animation> animations;
    std::string currentState;
    int currentFrame;
    float timer;

public:
    SpriteAnimator() : currentState(""), currentFrame(0), timer(0.0f) {}

    void addAnimation(const std::string& state, const std::vector<Rectangle>& frames, float frameTime = 0.1f, bool loop = true) {
        animations[state] = Animation(frames, frameTime, loop);
    }

    void setState(const std::string& state) override {
        if (state != currentState) {
            currentState = state;
            currentFrame = 0;
            timer = 0.0f;
        }
    }

    void update(float dt) override {
        auto it = animations.find(currentState);
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
    Rectangle getCurrentFrame() const override {
        auto it = animations.find(currentState);
        if (it == animations.end() || it->second.frames.empty()) {
            return Rectangle{ 0, 0, 16, 16 }; // fallback
        }
        const auto& frames = it->second.frames;
        int idx = currentFrame < (int)frames.size() ? currentFrame : 0;
        return frames[idx];
    }

    std::string getState() const override { return currentState; }

    // Clear all animations (used when switching power states)
    void clearAnimations() override {
        animations.clear();
        currentFrame = 0;
        timer = 0.0f;
    }
};

#endif // SPRITE_ANIMATOR_H
