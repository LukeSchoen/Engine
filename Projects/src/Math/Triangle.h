#ifndef Triangle_h__
#define Triangle_h__
#include "Maths.h"
#include "Sphere.h"

struct Triangle
{
  vec3 p1, p2, p3;
  vec3 normal;
  vec3 centrePos;
  float sqrBoundSize;
  Triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);
  const vec3 *getVert() const;
  bool PointInTriangle(vec3 point) const;
  float OnSweepSphere(const Sphere &sphere, const vec3 &velocity) const;
  float OnRay(const Ray &ray) const;
  float Area();
};

#endif // Triangle_h__
