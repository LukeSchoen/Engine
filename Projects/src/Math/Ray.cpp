#include "Ray.h"

Ray::Ray(const vec3 &_position, const vec3 &_direction)
{
  position = _position;
  direction = _direction;
}

float Ray::OnSphere(const Sphere &sphere) const
{
  return sphere.OnRay(*this);
}

float Ray::OnPlane(const Plane &plane) const
{
  float projection = -plane.normal.DotProduct(plane.position);
  float numerator = plane.normal.DotProduct(position) + projection;
  float demoniator = plane.normal.DotProduct(direction);
  if (demoniator == 0)  // plane is anisotropic with ray!
    return -1.0f;
  return -(numerator / demoniator);
}
