#include "Ray.h"

Ray::Ray()
{
	origin = Vec3f(0.0, 0.0, 0.0);
}

Ray::Ray(Vec3f _origin, Vec3f _position)
{
	origin = _origin;
	position = _position;
}
