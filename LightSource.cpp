#include "LightSource.h"

void polar2Cartesian (float phi, float theta, float r, float & x, float & y, float & z) {
    x = r * sin (theta) * cos (phi);
    y = r * sin (theta) * sin (phi);
    z = r * cos (theta);
}

LightSource::LightSource() : position(Vec3f(1.0,0.0,0.0)),
    color(Vec3f(1.f,1.f,1.f)), intensity(1.0) {}

LightSource::LightSource(Vec3f _position, Vec3f _color, float _intensity) :
    position(_position), color(_color), intensity(_intensity) {}

Vec3f LightSource::getPosition()
{
    float x, y, z;
    polar2Cartesian(position[2], position[1], position[0], x, y, z);
    return Vec3f(x,y,z);
}

Vec3f LightSource::getColor()
{
    return color;
}

void LightSource::addR(float r)
{
    position[0] = std::fmax(0.0, position[0] + r);
}

void LightSource::addPhi(float phi)
{
    position[1] = std::fmod(position[1] + phi, 2*M_PI);
}

void LightSource::addTheta(float theta)
{
    position[2] = std::fmod(position[2] + theta, 2*M_PI);
}

void LightSource::addRed(float red)
{
    color[0] = std::fmin(1.0, color[0] + red);
}

void LightSource::addGreen(float green)
{
    color[1] = std::fmin(1.0, color[1] + green);
}

void LightSource::addBlue(float blue)
{
    color[2] = std::fmin(1.0, color[2] + blue);
}

void LightSource::addIntensity(float _intensity)
{
    intensity += _intensity;
}
