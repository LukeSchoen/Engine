#ifndef Plane_h__
#define Plane_h__
#include "Maths.h"

struct Plane
{
  vec3 position;
  vec3 normal;

  bool PointIsInfront(vec3 point);

  float RayOnPlane(Ray ray);

  vec3 MovePointToPlane(const vec3 &point) const;
  
  float DistToPlane(vec3 point);

  Plane();

  Plane(const vec3 &v1, const vec3 &v2, const vec3 &v3);

  Plane(const vec3 &_position, const vec3 &_normal);
};

#endif // Plane_h__
