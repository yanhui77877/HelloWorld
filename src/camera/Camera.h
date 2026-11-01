#pragma once

#include <GraphicsMath.h>

class Camera
{
public:
    explicit Camera(const Vec3& initialPosition);

    void rotate(float mouseOffsetX, float mouseOffsetY);
    void move(float forwardInput, float rightInput, float deltaTime);

    Vec3 getPosition() const;
    Vec3 getFront() const;
    Mat4 getViewMatrix() const;

private:
    Vec3 position;
    Vec3 worldUp;
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
};
