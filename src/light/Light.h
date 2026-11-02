#pragma once
#include <GraphicsMath.h>

class Light
{
    Vec3  position;
    Vec3  color;
    float intensity;

public:
    Light(const Vec3& position, 
          const Vec3& color, 
          float intensity);

    Vec3 
    getPosition() const;

    Vec3 
    getColor() const;

    float 
    getIntensity() const;
};