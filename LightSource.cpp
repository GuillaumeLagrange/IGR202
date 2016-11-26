#include "LightSource.h"

LightSource::LightSource()
{
    position = Vec3f(0,0,0);
    color = Vec3f(1.f,1.f,1.f);
}

LightSource::LightSource(float x, float y, float z)
{
    position = Vec3f(x,y,z);
}

void LightSource::setPosition(float x, float y, float z)
{
  position = Vec3f(x,y,z);
}

void LightSource::setColor(float r, float g, float b)
{
  color = Vec3f(r,g,b);
}

Vec3f LightSource::getPosition()
{
  return position;
}

Vec3f LightSource::getColor()
{
  return color;
}

//void LightSource::setPosition(float x, float y, float z)
//{
//	position
//}
