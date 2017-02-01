#pragma once

#include "Vec3.h"
#include <vector>

class Ray {
private :
	Vec3f origin;
	Vec3f direction;

public :
	Ray();
	Ray(Vec3f, Vec3f);
	bool rayTriangleInter(Vec3f, Vec3f, Vec3f);
};
