#include "Plane.h"


bool Plane::PointIsInfront(vec3 point)
{
  vec3 direction = position - point;
  double distFromPlane = direction.DotProduct(normal);
  return (distFromPlane > 0.f);
}

float Plane::RayOnPlane(Ray ray)
{
  double projection = -normal.DotProduct(position);
  double numerator = normal.DotProduct(ray.position) + projection;
  double demoniator = normal.DotProduct(ray.direction);
  if (demoniator == 0)  // plane is anisotropic with ray!
    return -1.0f;
  return -(numerator / demoniator);
}

vec3 Plane::MovePointToPlane(const vec3 &point) const
{
  Ray ray(point, vec3() - normal);
  float dist = ray.OnPlane(*this);
  return point - normal * dist;
}

float Plane::DistToPlane(vec3 point)
{
  vec3 onPlane = MovePointToPlane(point);
  return (onPlane - point).Length();
}

Plane::Plane(const vec3 &v1, const vec3 &v2, const vec3 &v3)
{
  position = v1;
  normal = ((v2 - v1).CrossProduct(v3 - v1)).Normalized();
}

Plane::Plane(const vec3 &_position, const vec3 &_normal)
{
  position = _position;
  normal = _normal;
}

Plane::Plane()
{

}
