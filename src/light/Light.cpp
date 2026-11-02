#include <Light.h>

Light::Light(const Vec3& position, 
			 const Vec3& color, 
			 float intensity):
	position(position),
	color(color),
	intensity(intensity)
{}

Vec3 
Light::getPosition() const
{
	return Vec3(this->position);
}

Vec3 
Light::getColor() const
{
	return Vec3(this->color);
}

float 
Light::getIntensity() const
{
	return this->intensity;
}
