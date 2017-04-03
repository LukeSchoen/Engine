#ifndef Ray_h__
#define Ray_h__
#include "Maths.h"

struct Ray
{
  Ray(const vec3 &_position, const vec3 &_direction);

  float OnSphere(const Sphere &sphere) const;

  float OnPlane(const Plane &plane) const;

  vec3 position;
  vec3 direction;
};

#endif // Ray_h__
