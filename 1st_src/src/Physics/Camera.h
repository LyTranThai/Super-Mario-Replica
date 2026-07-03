#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"

class GameCamera {
private:
    Vector2 position;
    Vector2 viewportSize;
    float leftBoundary;
    float rightBoundary;

public:
    GameCamera();
    GameCamera(Vector2 size, float leftBound, float rightBound);

    void update(Vector2 targetPos);
    Vector2 applyOffset(Vector2 worldPos) const;
    Rectangle applyOffsetToRect(Rectangle worldRect) const;

    Vector2 getPosition() const { return position; }
    void setBoundaries(float leftBound, float rightBound);
};

#endif // CAMERA_H
