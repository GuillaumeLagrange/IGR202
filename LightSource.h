#pragma once

#include "Vec3.h"

class LightSource {
private :
    Vec3f position;
    Vec3f color;
    float intensity;

public :
    LightSource();
    LightSource(Vec3f _position, Vec3f _color, float _intensity);
    Vec3f getPosition();
    Vec3f getColor();

    void addR(float r);
    void addPhi(float phi);
    void addTheta(float theta);

    void addRed(float red);
    void addGreen(float geen);
    void addBlue(float blue);

    void addIntensity(float _intensity);
};
