#include <cmath>

#include "Ray.h"

Ray::Ray()
{
	origin = Vec3f(0.0, 0.0, 0.0);
	direction = Vec3f(1.0, 1.0, 1.0);
}

Ray::Ray(Vec3f _origin, Vec3f _direction)
{
	origin = _origin;
	direction = _direction;
}

bool Ray::rayTriangleInter(Vec3f p0, Vec3f p1, Vec3f p2)
{
	Vec3f e0 = p1 - p0;
	Vec3f e1 = p2 - p0;
	Vec3f n = normalize(cross(e0, e1));
	Vec3f q = cross(direction, e1);
	float a = dot(e0, q);

	if (dot(n, direction) >= 0 || std::abs(a) < 0.001)
		return false;

	Vec3f s = (origin - p0) / a;
	Vec3f r = cross(s, e0);
	float b0 = dot(s, q);
	float b1 = dot(r, direction);
	float b2 = 1.f - b0 - b1;

	if (b0 < 0 || b1 < 0 || b2 < 0)
		return false;

	float t = dot(e1, r);

	if (t >= 0)
		return true;

	return false;
}
