#ifndef Sphere_h__
#define Sphere_h__
#include "Maths.h"

struct Sphere
{
  vec3 position;
  float radius;

  Sphere();

  Sphere(const vec3 &position, const float &radius = 1.0f);

  bool HitsPoint(const vec3 &pointPosition) const;

  bool HitsRay(const Ray &ray) const;

  bool HitsSphere(const Sphere &sphere) const;

  float OnRay(const Ray &ray) const;

};

#endif // Sphere_h__
