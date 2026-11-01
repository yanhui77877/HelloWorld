#include <Camera.h>

Camera::Camera(const Vec3& initialPosition)
    : position(initialPosition),
      worldUp{0.0f, 1.0f, 0.0f},
      yaw(-90.0f),
      pitch(0.0f),
      movementSpeed(2.0f),
      mouseSensitivity(0.1f)
{
}

void Camera::rotate(float mouseOffsetX, float mouseOffsetY)
{
    yaw += mouseOffsetX * mouseSensitivity;
    pitch += mouseOffsetY * mouseSensitivity;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void Camera::move(float forwardInput, float rightInput, float deltaTime)
{
    Vec3 forward = getFront();
    Vec3 right = normalize(cross(forward, worldUp));
    Vec3 direction = add(
        ::scale(forward, forwardInput),
        ::scale(right, rightInput)
    );

    position = add(
        position,
        ::scale(normalize(direction), movementSpeed * deltaTime)
    );
}

Vec3 Camera::getPosition() const
{
    return position;
}

Vec3 Camera::getFront() const
{
    float yawRadians = yaw * 3.14159265f / 180.0f;
    float pitchRadians = pitch * 3.14159265f / 180.0f;

    return normalize(Vec3{
        std::cos(pitchRadians) * std::cos(yawRadians),
        std::sin(pitchRadians),
        std::cos(pitchRadians) * std::sin(yawRadians)
    });
}

Mat4 Camera::getViewMatrix() const
{
    return Mat4::viewFromDirection(position, getFront(), worldUp);
}
